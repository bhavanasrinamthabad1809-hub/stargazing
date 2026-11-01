
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

const int BTN_START   = 0;
const int BTN_DONE    = 1;
const int BTN_NOTDONE = 3;
const int BUZZER_PIN  = 10;
const int SDA_PIN     = 5;
const int SCL_PIN     = 6;


const bool TEST_MODE = false;
const char* AP_NAME = "TaskClock";
const char* AP_PASS = "panda1234";
const int MAX_TASKS = 10;
const unsigned long START_GRACE_MS = 10UL * 60 * 1000;
const unsigned long ALARM_MAX_MS   = 2UL * 60 * 1000;
const unsigned long DEBOUNCE_MS    = 30;

// ---------------- hardware objects ----------------
Adafruit_SSD1306 display(128, 64, &Wire, -1);
RTC_DS3231 rtc;
WebServer server(80);

// ---------------- tasks ----------------
struct TaskItem {
  String name;
  int startMin;    // minutes since midnight, so 18:30 = 1110
  int endMin;
  int status;      // 0 waiting, 1 done, 2 not done, 3 missed
};
TaskItem tasks[MAX_TASKS];
int taskCount = 0;
int stars = 0;
int savedDay = 0;  // the date the task statuses belong to, like 20261101

// ---------------- buttons ----------------
struct PushButton {
  int pin;
  bool stableState;
  bool lastReading;
  unsigned long lastChange;
};
PushButton startBtn   = { BTN_START,   HIGH, HIGH, 0 };
PushButton doneBtn    = { BTN_DONE,    HIGH, HIGH, 0 };
PushButton notDoneBtn = { BTN_NOTDONE, HIGH, HIGH, 0 };

// ---------------- sounds ----------------
struct Note {
  int freq;   // 0 means silence
  int ms;
};
Note bootNotes[]       = { {2000, 100}, {0, 50}, {2600, 120} };
Note startAlertNotes[] = { {1500, 150}, {0, 100}, {2000, 150}, {0, 100}, {2500, 300} };
Note sirenNotes[]      = { {2048, 300}, {1500, 300} };
Note doneNotes[]       = { {2000, 120}, {2500, 120}, {3000, 250} };
Note failNotes[]       = { {800, 250}, {500, 450} };

#define COUNT(a) (sizeof(a) / sizeof(a[0]))

Note* melody = NULL;
int melodyLength = 0;
int melodyIndex = 0;
bool melodyLoops = false;
unsigned long noteEndsAt = 0;


enum ClockState {
  STATE_WAITING,      
  STATE_START_ALERT,  
  STATE_RUNNING,      
  STATE_DEADLINE,     
  STATE_RESULT,       
  STATE_SKY         
};
ClockState state = STATE_WAITING;

int current = -1;                 
unsigned long stateStartedMs = 0;
unsigned long nextRemindMs = 0;
unsigned long lastDrawMs = 0;
unsigned long lastClockReadMs = 0;
int runStartSecs = 0;
bool lastResultWasDone = false;
unsigned long starSeed = 0;

DateTime currentTime;
int nowSecs = 0;                 

bool setupMode = false;
unsigned long restartAt = 0;

bool wasPressed(PushButton &b);
void startMelody(Note *m, int length, bool loops);
void updateSound();
void stopSound();
void changeState(ClockState newState);
void finishTask(bool wasDone);
void readClock();
void checkNewDay();
int  findNextTask();
void saveTasks();
void loadTasks();
void saveStars();
void loadStars();
void loadTestTasks();
void sortTasks();
int  toMinutes(String hhmm);
String twoDigits(int n);
String shortName(String s, int maxLen);
void startSetupMode();
void handleRoot();
void handleSave();
void showMessage(String a, String b, String c);
void printCentered(String text, int y, int size);
void printName(String name, int y);
void drawScreen();
void drawWaiting();
void drawStartAlert();
void drawRunning();
void drawDeadline();
void drawResult();
void drawSky();
int  starRand(int range);

void loop() {


  if (setupMode) {
    server.handleClient();
    if (restartAt != 0 && millis() > restartAt) {
      ESP.restart();
    }
    return;
  }

  updateSound();  

  
  bool startPressed   = wasPressed(startBtn);
  bool donePressed    = wasPressed(doneBtn);
  bool notDonePressed = wasPressed(notDoneBtn);

  
  if (millis() - lastClockReadMs > 250) {
    lastClockReadMs = millis();
    readClock();
  }

  switch (state) {

    case STATE_WAITING:
      
      for (int i = 0; i < taskCount; i++) {
        if (tasks[i].status != 0) continue;            

        int startSecs = tasks[i].startMin * 60;
        int endSecs   = tasks[i].endMin * 60;

        if (nowSecs >= endSecs) {
          tasks[i].status = 3;                         
          saveTasks();
        } else if (nowSecs >= startSecs) {
          current = i;
          startMelody(startAlertNotes, COUNT(startAlertNotes), false);
          nextRemindMs = millis() + 60000;             
          changeState(STATE_START_ALERT);
          break;
        }
      }
      
      if (donePressed) {
        changeState(STATE_SKY);
      }
      break;

    case STATE_START_ALERT:
      if (startPressed) {
        runStartSecs = nowSecs;
        stopSound();
        changeState(STATE_RUNNING);
      }
      else if (nowSecs >= tasks[current].endMin * 60 || millis() - stateStartedMs > START_GRACE_MS) {
        // never pressed START in time
        tasks[current].status = 3;
        saveTasks();
        stopSound();
        current = -1;
        changeState(STATE_WAITING);
      }
      else if (millis() > nextRemindMs) {
        startMelody(startAlertNotes, COUNT(startAlertNotes), false);
        nextRemindMs = millis() + 60000;
      }
      break;

    case STATE_RUNNING:
      if (donePressed) {
        finishTask(true);                              
      }
      else if (notDonePressed) {
        finishTask(false);                             
      }
      else if (nowSecs >= tasks[current].endMin * 60) {
        startMelody(sirenNotes, COUNT(sirenNotes), true);   
        changeState(STATE_DEADLINE);
      }
      break;

    case STATE_DEADLINE:
      if (donePressed) {
        finishTask(true);
      }
      else if (notDonePressed) {
        finishTask(false);
      }
      else if (millis() - stateStartedMs > ALARM_MAX_MS) {
        finishTask(false);                             
      }
      break;

    case STATE_RESULT:
      if (millis() - stateStartedMs > 3000) {
        current = -1;
        changeState(STATE_WAITING);
      }
      break;

    case STATE_SKY:
      if (millis() - stateStartedMs > 5000 || startPressed || donePressed || notDonePressed) {
        changeState(STATE_WAITING);
      }
      break;
  }

  if (millis() - lastDrawMs > 250) {
    lastDrawMs = millis();
    drawScreen();
  }
}

void setup() {
  Serial.begin(115200);

  
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_DONE, INPUT_PULLUP);
  pinMode(BTN_NOTDONE, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);


  Wire.begin(SDA_PIN, SCL_PIN);

  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, false)) {
    Serial.println("OLED not found, check the wiring");
  }
  display.setTextWrap(false);
  display.clearDisplay();
  display.display();

  
  if (!rtc.begin()) {
    Serial.println("RTC not found, check the wiring");
    showMessage("RTC not found", "check the wiring", "");
    delay(3000);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, time needs setting");
    showMessage("Time not set!", "Hold START while", "plugging in USB");
    delay(4000);
  }


  if (!LittleFS.begin(true)) {
    Serial.println("Storage problem");
  }
  loadTasks();
  loadStars();
  readClock();
  if (TEST_MODE) {
    loadTestTasks();
  }

  
  if (digitalRead(BTN_START) == LOW) {
    startSetupMode();
    return;
  }

  Serial.println("Panda task clock ready");
  startMelody(bootNotes, COUNT(bootNotes), false);
  changeState(STATE_WAITING);
}




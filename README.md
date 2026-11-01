#  Stargazing

**A panda-shaped task clock that nags me when it's time to start, yells when time's up, and gives me a star if I actually did the thing.**



## Why I made this

I am a professional procrastinator.

I don't mean "oh, I put things off a little." I mean I sit down to read a book for 30 minutes, and somehow it is two hours later and I have watched a video about how bridges are built. Phone alarms don't work on me, because I swipe them away without even waking up properly. To-do apps don't work either, because opening an app is already a task I'm avoiding.

So I asked myself: what if the thing that nags me was a *physical object* that sits on my desk, can't be swiped away, and is kind of cute so I don't hate it?

That's Stargazing. Every task has a **start time** and a **deadline**. At the start time it beeps at me. At the deadline it sounds an alarm. In between, I either press **DONE** and get a star in my little night sky, or I press **NOT DONE** and get nothing. It turns out I will do a surprising amount of work for a tiny pixel star.

---

## What it does

- Shows the **time and date** like a normal desk clock.
- Holds up to **10 tasks**, each with a name, a start time and a deadline.
- **Beeps at the start time** (three rising notes, repeated every minute until I press START).
- Shows a **countdown and a progress bar** while a task is running.
- Sounds a **siren at the deadline** until I press DONE or NOT DONE.
- Adds a **star to my star sky** for every task I finish, and the stars are saved even when it's unplugged.
- Lets me **add tasks from my phone** over Wi-Fi, with no app to install.
- Keeps the correct time even when the power is off, thanks to a battery-backed clock chip.
- Repeats my tasks every day, so I only have to type them in once.

**The three buttons** (left to right on the panda's belly): `start`, `not done`, `done`.
Pressing `done` on the home screen shows my star sky.

---

## How it works (the short version)

1. I load my tasks from my phone (see [How to use](#how-to-use)).
2. The ESP32 keeps checking the time from the RTC.
3. When a task's start time arrives → **start alert**.
4. I press **START** → countdown begins.
5. When the deadline arrives → **siren**.
6. I press **DONE** (⭐ +1) or **NOT DONE** (no star), and it moves on to the next task.

What if I ignore it?
- I never press START: after 10 minutes the task counts as **missed**.
- I ignore the siren: after 2 minutes it counts as **not done**.

(It really does not let me get away with anything. That's the point.)

---

## Parts I used, and why

 Part What it does  Why I picked it 
|**ESP32-C3 SuperMini** 
The brain. Runs the code, makes the Wi-Fi page, reads the buttons  Tiny, cheap, USB-C, Wi-Fi built in, works with Arduino 
 **0.96" SSD1306 OLED (I2C)** 
 Shows the clock, task, countdown and star sky | Only 2 data wires, cheap, easy to read, perfect for pixel stars 
**DS3231 RTC module**  
Counts the real time all the time  Very accurate, and the coin battery keeps it running when unplugged. The ESP32 forgets the time when it loses power, this doesn't 
 **LIR2032 coin battery** 
 Backup power for the RTC The module has a charging circuit built for a rechargeable cell, so a normal CR2032 isn't the right choice 
 **5V passive buzzer**
 Makes all the sounds | A passive one can play different pitches, so the start reminder, siren, happy jingle and sad tone all sound different 
S8050 NPN transistor (Q1)
A switch for the buzzer An ESP32 pin can only supply a few milliamps, and the buzzer needs a lot more. The pin controls the transistor, and the transistor drives the buzzer
**R1 1kΩ** 
Goes between GPIO10 and the transistor's base Limits the current going into the base so the ESP32 pin stays safe
**R2 10kΩ** 
Pulls the base to GND  Stops the buzzer from chirping randomly while the ESP32 is booting 
**D1 1N4148 diode**
Across the buzzer  A buzzer has a coil inside, and coils make a voltage spike when switched off. The diode gives that spike a harmless path 
 **3 × 12 mm tactile switches**  
 Start, Not Done, Done  Big and clicky, and I already had them (they came in a combo pack with colored caps) 
 **Custom PCB** 
 Holds everything together  I wanted to actually *design* a board instead of making a mess of jumper wires 
**3D-printed panda case**  
The cute part  Because I won't ignore a panda 

**Power:** just USB-C from a charger or power bank. No big battery, because it lives on my desk. The only battery is the tiny one for the RTC.

### schematic pcb


## How the firmware works

The code is written in Arduino C++ 

**Main idea:**:

| State | What's happening | How it ends |
|---|---|---|
| `WAITING` | Home screen: time, date, next task | A task's start time arrives, or I press DONE to see the star sky |
| `START_ALERT` | Beeping, "TIME TO START" | I press START, or 10 minutes pass (missed) |
| `RUNNING` | Countdown and progress bar | Deadline arrives, or I press DONE / NOT DONE early |
| `DEADLINE` | Siren, "TIME'S UP!" | I press DONE or NOT DONE, or 2 minutes pass (not done) |
| `RESULT` | "GREAT JOB!" or "NOT DONE" for 3 seconds | Timer ends, back to `WAITING` |
| `SKY` | My stars | 5 seconds, or any button |


**First time / changing tasks**
1. Unplug the clock, then plug it in **while holding START**.
2. On your phone, join the Wi-Fi **`TaskClock`** (password **`panda1234`**, you can change both in the code).
3. Open **`192.168.4.1`** in your browser.
4. Add your tasks (name, start, deadline) and press **Save to clock**.
5. It restarts by itself and you're done.

**Every day**
Just look at it. It will tell you what to do

## Bill of materials (BOM)

| # | Ref | Part | Value / spec | Qty | Footprint / notes |
|---|---|---|---|---|---|
| 1 | U2 | ESP32-C3 SuperMini | | 1 |  |
| 2 | DS1 | 0.96" OLED, SSD1306, I2C | 128×64, 4-pin | 1 |  |
| 3 | J1 | Pin header, 1×04 | 2.54 mm | 1 | OLED connection |
| 4 | — | DS3231 RTC module | ZS-042 type, AT24C32 | 1 | Use the 6-pin side |
| 5 | J2 | Pin socket, 1×06 | 2.54 mm | 1 | `PinSocket_1x06_P2.54mm_Vertical` |
| 6 | — | Jumper wires | male-to-female | 4 | RTC to J2: SCL, SDA, VCC, GND |
| 7 | — | LIR2032 battery | 3.6 V rechargeable | 1 | For the RTC module |
| 8 | BZ1 | Passive buzzer | 5 V, 12×8 mm, **6 mm pin pitch** | 1 |  |
| 9 | Q1 | S8050 NPN transistor | **TO-92** (not SOT-23) | 1 | ` |
| 10 | D1 | 1N4148 diode | | 1 | `D_DO-35_SOD27_P7.62mm`, stripe toward +5V |
| 11 | R1 | Resistor | 1 kΩ, 1/4 W | 1 | `R_Axial_DIN0207_L6.3mm_P10.16mm` |
| 12 | R2 | Resistor | 10 kΩ, 1/4 W | 1 | `R_Axial_DIN0207_L6.3mm_P10.16mm` |
| 13 | SW1–SW3 | Tactile switch | 12×12 mm, 4-pin | 3 | `SW_PUSH-12mm`, measure the real pins first |
| 14 | — | Switch caps | round, 12 mm | 3 | White and black cap theme |
| 15 | — | PCB | 2-layer, ~100×78 mm | 1 | KiCad project in `hardware/` |
| 16 | — | M3 screws + standoffs | | 4 each | Corner mounting holes |
| 17 | — | USB-C cable + 5 V charger | | 1 | Power and programming |
| 18 | — | 3D-printed panda case | | 1 | Designed in Onshape (`enclosure/`) |

## Things that went wrong (so you don't repeat them)

I'm putting these here on purpose, because I made all of them:

- **Wrong transistor package.** I almost bought the SOT-23 S8050 (tiny, surface-mount, different pin order) instead of the through-hole TO-92 one my footprint was for.
- **Wrong transistor symbol.** `Q_NPN_ECB` and `Q_NPN_EBC` look the same but the pins are in a different order. A wrong choice means the buzzer never turns on, and you can't fix a printed board.
- **Wrong buzzer footprint.** The library one has 7.6 mm between the pins. My buzzer has 6 mm. Always measure.
- **Two PWR_FLAGs on GND** made KiCad throw errors. You only need one per power net.
- **Active vs passive buzzer.** I bought a passive one, so the code has to generate the tones, not just switch it on.

## Made with

KiCad, Onshape, , Arduino IDE, and a lot of avoiding my homework.

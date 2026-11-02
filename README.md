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

**The three buttons** (left to right on the panda's belly): start not done done
Pressing `done` on the home screen shows my star sky.

---

## How it works 

1. I load my tasks from my phone 
2. The ESP32 keeps checking the time from the RTC.
3. When a task's start time arrives → **start alert**.
4. I press **START** → countdown begins.
5. When the deadline arrives → **siren**.
6. I press **DONE** ( +1) or **NOT DONE** (no star), and it moves on to the next task.

What if I ignore it?
- I never press START: after 10 minutes the task counts as **missed**.
- I ignore the siren: after 2 minutes it counts as **not done**.

(It really does not let me get away with anything. That's the point.)



###  pcb
schematic for my pcb 
<img width="1920" height="1080" alt="Screenshot 2026-11-01 214705" src="https://github.com/user-attachments/assets/2ae9ec5c-d95f-4f67-9c3d-0795be7e12a1" />

pcb 
<img width="1920" height="1080" alt="Screenshot 2026-11-01 214755" src="https://github.com/user-attachments/assets/b4fe2c5b-f856-4b13-b285-f639c57abf4b" />

3d model of my pcb
<img width="1920" height="1080" alt="Screenshot 2026-11-01 234520" src="https://github.com/user-attachments/assets/35305021-4898-4b6a-9e5a-dfaaae265709" />



## How the firmware works

The code is written in Arduino C++ 

**Main idea:**:

| State | What's happening | How it ends |
|---|---|---|
| WAITING | Home screen: time, date, next task | A task's start time arrives, or I press DONE to see the star sky |
| START_ALERT | Beeping, "TIME TO START" | I press START, or 10 minutes pass (missed) |
| RUNNING | Countdown and progress bar | Deadline arrives, or I press DONE / NOT DONE early |
| DEADLINE | Siren, "TIME'S UP!" | I press DONE or NOT DONE, or 2 minutes pass (not done) |
| RESULT | "GREAT JOB!" or "NOT DONE" for 3 seconds | Timer ends, back to `WAITING` |
| SKY | My stars | 5 seconds, or any button |

**cad time:**:
bottom case 

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/51da6545-f9c7-4d8c-bc59-7cde9270a3f5" />

different view 

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/ac3dd679-c100-449a-93e4-cc475e0d090d" />

with pcb view

<img width="1496" height="732" alt="image" src="https://github.com/user-attachments/assets/89469e6b-856c-424e-835b-a04203da65d7" />

top case 

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/2052f48a-dbd4-4e41-8827-fe78429ef090" />

top view 

<img width="1327" height="760" alt="image" src="https://github.com/user-attachments/assets/0e31833a-5cc0-4c74-a3c3-bad047d92b30" />


top back side view 

<img width="1401" height="767" alt="image" src="https://github.com/user-attachments/assets/0487fef6-dc53-46e4-8db9-6c0210b98436" />

full box view 

<img width="1347" height="650" alt="image" src="https://github.com/user-attachments/assets/86f19fb4-1dd8-4a1f-b5d1-2cb6ebb3b982" />


**First time / changing tasks**
1. Unplug the clock, then plug it in **while holding START**.
2. On your phone, join the Wi-Fi **TaskClock** (password **panda1234**, you can change both in the code).
3. Open  in your browser.
4. Add your tasks (name, start, deadline) and press **Save to clock**.
5. It restarts by itself and you're done.

**Every day**
Just look at it. It will tell you what to do

## bill of materials 

| Component    | Quantity | Description         | Source                       | Price |
| ------------ | -------: | ------------------- | ---------------------------- | ----: |
| ESP32-C3 supermini    |        1 | Microcontroller     | [link](https://www.amazon.in/ESP32-Mini-Development-Board-Compatibility/dp/B0GQC8T6HV/ref=sr_1_8?crid=1YXDR902MUSIG&dib=eyJ2IjoiMSJ9.sGPT0TLrTVc4fjCA5M25kfKz9ewU1R1wIuoBckeM0x8HEPyzj9XwnklifiFo_89Bbb9mzE4WWo2DGeTRlLvV83Cb8s-klzuzTJVASNJNhv7Nqps-rZ-4lXOy6xMqxsJsVY7faohUpmPXMVEaXuAB03q0ysA6oRApeUPT93Cd3TyWsNSXOMPB1vQdJxj7mprZZuFzUoUwf0fJYcKTRnwLri9shof3BKq_yP47YysvE1c.27zLobKgGvnCgzQQ009P8C8V5wGSJ3_S2HBPNDYdVg8&dib_tag=se&keywords=esp32-c3+supermini&qid=1793557862&sprefix=esp32-c%2Caps%2C314&sr=8-8) | $8.21 |
| OLED Display |        1 | Small display       | [link](https://www.amazon.in/gp/product/B07Q7VZ716/ref=sw_img_1?smid=&psc=1) |$3.16 |
| 10K Resistor |        1 | Pull-up resistor    | [link](https://robocraze.com/products/10k-resistor-pack-of-10) ||
| switches  |        3 | User input buttons  | [link](https://www.amazon.in/gp/product/B0D9NGL25F/ref=ox_sc_act_title_6?smid=AJ6SIZC8YQDZX&psc=1) | $3.25 |
|  buzzer |        1 | alarm  | [link](https://robu.in/product/1-month-warranty-1248/) | $0.53 |
| PCB          |        1 | Custom-designed PCB | [link](https://jlcpcb.com/)  | |
| coin  battery | 1 | to work without the connection  | [Link]() |  |
| rtc module | 1 | time tracking | [Link]() |  |
| S8050 transistor | 1 | works as a switch to buzzer | [Link]() |  |
| 1k resistor | 1 | to work without the connection  | [Link]() |  |
| jumper wires | 1 | to solder with rtc  | [Link]() |  |
| 1N4148 diode | 1 | to solder with rtc  | [Link](https://robu.in/product/1n4148-multicomp-pro-1n4148-diode-ultrafast-recovery-150ma-75v-do-204ah-2-2/) | $0.16 |
| m3screws | 1 | to solder with rtc  | [Link]() |  |
| pcb case | 1 | to place pcb  | [Link](https://robu.in/product/numaker-abs-filament-pure-white-1-75-mm-1-kg/) | $8.21  |


## Made with

KiCad, Onshape, , Arduino IDE, and a lot of avoiding my homework.

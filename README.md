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




**First time / changing tasks**
1. Unplug the clock, then plug it in **while holding START**.
2. On your phone, join the Wi-Fi **TaskClock** (password **panda1234**, you can change both in the code).
3. Open  in your browser.
4. Add your tasks (name, start, deadline) and press **Save to clock**.
5. It restarts by itself and you're done.

**Every day**
Just look at it. It will tell you what to do



## Made with

KiCad, Onshape, , Arduino IDE, and a lot of avoiding my homework.

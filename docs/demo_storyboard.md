# Demo Storyboard — Smart EV Charging Station

> This storyboard describes the frames for a demonstration GIF/video showing the system in operation. Capture these scenes from the physical prototype to create a compelling portfolio asset.

---

## Scene 1 — System Boot (0:00 – 0:05)

**What to capture:** Power cable connected to the breadboard prototype. LCD lights up.

**LCD shows:**
```
Smart EV Charger
Initialising...
```
Then:
```
GSM Init...
GSM OK
```

**Voiceover / caption:** "Arduino Nano boots, initializes GSM SIM900A module."

---

## Scene 2 — IDLE State (0:05 – 0:10)

**What to capture:** LCD showing idle state. Green LED off. No relay sound.

**LCD shows:**
```
EV Charger Ready
Send UPI Payment
```

**Caption:** "System awaits UPI SMS payment from user's phone."

---

## Scene 3 — User Sends Payment (0:10 – 0:20)

**What to capture:** Phone screen showing Google Pay / PhonePe sending ₹50 to station number. SMS confirmation appearing on phone.

**Phone screen:** User types ₹50 and taps "Pay" to station number.
**Bank SMS received on user phone:** `"Payment of Rs 50 received..."`
**Same SMS delivered to station SIM900A.**

**Caption:** "User sends ₹50 via UPI. Bank SMS reaches station SIM."

---

## Scene 4 — Payment Verification (0:20 – 0:28)

**What to capture:** Arduino polls SIM900A. LCD updates.

**LCD shows:**
```
Payment: Rs50
Verifying...
```

**Caption:** "Arduino parses SMS, extracts ₹50, validates payment keywords."

---

## Scene 5 — Pre-Charge Safety Check (0:28 – 0:35)

**What to capture:** Multimeter showing battery pack voltage (~11.2V = 60% SOC). LM35 showing ~28°C.

**LCD shows:**
```
Safety Check OK
Starting Charge
```

**Caption:** "Voltage and temperature within safe range. SSR activates."

---

## Scene 6 — Charging Active (0:35 – 1:00)

**What to capture:** SSR indicator LED on. Battery pack connected. Multimeter showing voltage slowly rising. LCD updating every 2 seconds.

**LCD cycles through:**
```
SOC:62% 11.3V
28°C  00:12
```
```
SOC:65% 11.4V
29°C  00:30
```
```
SOC:70% 11.5V
29°C  01:00
```

**Caption:** "Real-time SOC, voltage, temperature, and elapsed time on LCD. Updates every 2 seconds."

---

## Scene 7 — Temperature Warning Demonstration (Optional) (1:00 – 1:20)

**What to capture:** Warm object placed near LM35 to simulate overtemperature. Fault triggers.

**LCD shows:**
```
! FAULT !
Overtemperature
```

**Red LED turns on. Buzzer sounds 3 times. SSR turns off.**

**Caption:** "Thermal protection triggers immediately. SSR cuts charging. Fault logged to EEPROM."

**Recovery:** Remove heat source. System auto-recovers after 30 seconds.

```
EV Charger Ready
Send UPI Payment
```

**Caption:** "Auto-recovery when fault condition clears."

---

## Scene 8 — Session Complete (1:20 – 1:35)

**What to capture:** SOC reaching ~98%. SSR turns off. Buzzer beeps twice.

**LCD shows:**
```
Session Complete
SOC:98% 22min
```

**Caption:** "Charging completes when battery reaches target SOC. Session logged."

---

## Scene 9 — EEPROM Session Log (1:35 – 1:45)

**What to capture:** Serial monitor on laptop showing session records read from EEPROM.

**Serial output:**
```
=== Session Log ===
Record 1: Duration=22min SOC:62%->98% Fault=NONE
Record 2: Duration=8min  SOC:70%->75% Fault=OVERTEMP
Total sessions: 2
```

**Caption:** "Session history stored in EEPROM — persists across power cycles."

---

## GIF Creation Instructions

1. **Screen record** phone screen during payment send (Scene 3) using Android screen recorder
2. **Film prototype** with phone camera, horizontal orientation, good lighting
3. **Capture LCD** close-up separately (reflections are common — angle light source away)
4. **Combine scenes** using DaVinci Resolve (free) or Kdenlive
5. **Export GIF** at 480×270 resolution, 15fps, max 10MB for GitHub README embedding
6. **Upload** to `images/demo.gif` and reference in README

---

## Static Image Assets to Capture

| Filename | What to Photograph |
|---|---|
| `prototype_overview.jpg` | Full breadboard setup, all components labeled |
| `lcd_idle.jpg` | LCD showing "EV Charger Ready / Send UPI Payment" |
| `lcd_charging.jpg` | LCD showing SOC + voltage during active charge |
| `lcd_fault.jpg` | LCD showing fault message |
| `lcd_complete.jpg` | LCD showing session complete summary |
| `gsm_module.jpg` | SIM900A close-up with SIM card inserted |
| `bms_module.jpg` | BMS module close-up with cell connections |
| `ssr_closeup.jpg` | SSR with control wire and load wire labeled |
| `lm35_closeup.jpg` | LM35 temperature sensor on breadboard |
| `circuit_overview.jpg` | Full circuit from above, all wires visible |
| `payment_sms.jpg` | Phone screen showing UPI payment SMS confirmation |

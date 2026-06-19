# Project Summary for Resume & Applications

---

## Resume Bullet Points (Pick 3–5)

Use these under a "Projects" section. Start every bullet with a strong action verb.

---

### Version A — Technical Focus (for Embedded Systems / EV Roles)

**Smart EV Charging Station with BMS** | Arduino Nano, GSM SIM900A, C++ | *State-Level Exhibition*

- Designed and implemented a multi-layer battery protection system combining a commercial BMS hardware module with software-level voltage and temperature monitoring on an ATmega328P microcontroller (32KB flash, 2KB SRAM)
- Developed an SMS parsing algorithm in C++ to verify UPI payment confirmations received via GSM SIM900A AT commands, triggering a Solid State Relay within ~3 seconds of confirmed payment
- Engineered a voltage divider + 10-bit ADC pipeline for accurate 3S Li-ion pack voltage sensing, achieving ±0.1V accuracy across a 9V–12.6V measurement range
- Implemented a non-blocking state machine using millis()-based timers to concurrently handle SMS polling, sensor reads, LCD updates, and safety monitoring
- Designed EEPROM ring-buffer session logger storing up to 90 charging sessions (start SOC, end SOC, duration, fault code) persistently across power cycles

---

### Version B — Impact Focus (for TCS / Infosys / Accenture / General IT)

**Smart EV Charging Station with BMS** | Arduino Nano, GSM, Embedded C++ | *State-Level Exhibition*

- Built a fully functional smart EV charging controller using Arduino Nano, GSM SIM900A, and commercial BMS module — physically demonstrated at a State-Level Technical Exhibition
- Automated UPI payment verification via GSM SMS parsing, eliminating manual billing and enabling cashless charging at sub-₹3,000 hardware cost
- Implemented real-time battery safety monitoring with automatic fault detection (overvoltage, overtemperature) and SSR-based charging cutoff to protect lithium-ion battery packs
- Delivered complete working prototype within Arduino Nano's 32KB flash and 2KB SRAM constraints, demonstrating resource-efficient embedded systems design

---

### Version C — Concise One-Liner (for Applications with Word Limits)

> Built a GSM-enabled smart EV charging controller (Arduino Nano, SIM900A, BMS, SSR) with UPI payment verification via SMS parsing and multi-layer Li-ion battery protection; selected for State-Level Technical Exhibition.

---

## LinkedIn "About" Project Mention

> In my final year diploma project, I designed and built a Smart EV Charging Station with Battery Management System — an Arduino Nano controller that verifies UPI payments via GSM SMS, protects Li-ion batteries with hardware BMS + software monitoring, and controls charging via a Solid State Relay. The project was selected for a State-Level Technical Exhibition and demonstrated live with real UPI payments.

---

## Key Technical Skills This Project Demonstrates

| Skill | Evidence |
|---|---|
| Embedded C++ / Arduino | Firmware written in C++ within ATmega328P constraints |
| UART / Serial Communication | SoftwareSerial interface with SIM900A at 9600 baud |
| AT Command Programming | GSM module initialized and controlled via AT command set |
| ADC Sensing | Voltage divider + LM35 → ADC → calibrated physical units |
| State Machine Design | Non-blocking 6-state system (IDLE → CHARGE → FAULT etc.) |
| Battery Management | SOC estimation, overvoltage/overtemp protection, BMS integration |
| Relay / SSR Control | Digital output driving SSR for charging circuit switching |
| EEPROM Data Storage | Ring-buffer persistent logging of session records |
| Power Electronics Basics | Voltage divider design, relay selection, power supply rail splitting |
| Hardware Debugging | Physical prototype build and test including fault injection |
| Technical Documentation | Architecture diagrams, BMS design docs, GSM integration guide |

---

## Application-Specific Framings

### For TCS Digital / TCS Prime
> Demonstrates embedded systems fundamentals, hardware-software integration, and IoT-adjacent skills (GSM connectivity, sensor data pipeline, state machine firmware) relevant to TCS's Digital Transformation and Embedded Systems practices.

### For Infosys / Accenture / Capgemini / Wipro
> Shows ability to architect a complete end-to-end system (payment → verification → hardware actuation → feedback), debug physical hardware, and document technical work to professional standards — directly applicable to embedded and IoT project delivery roles.

### For EV Industry Internships (Ather, Tata Power EV, Hero Electric, etc.)
> Directly relevant: Li-ion BMS design principles, charging state machine, thermal protection logic, SOC estimation, and SSR-based charge control — all core to EV powertrain and charging system development.

### For Embedded Systems Internships
> Demonstrates real firmware delivery: constrained resource management (32KB flash), peripheral integration (UART, ADC, GPIO), non-blocking timer architecture, and hardware debugging on physical silicon — not simulation.

### For Higher Studies / Research Applications
> Shows independent technical project capability: system architecture from scratch, multi-module C++ firmware design, hardware-software co-design, and selection of commercially viable components for a real-world problem.

---

## GitHub Repository Link

> `https://github.com/prashanth-2369002/SmartEV-Charging-System`

Include this link in:
- Resume (Projects section)
- LinkedIn Featured section
- Job application "portfolio" fields
- College internship application forms

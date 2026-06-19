# State-Level Exhibition Slide Concepts

> Concept outline for the presentation slides used at the state-level technical exhibition. Recreate these slides in PowerPoint, Google Slides, or Canva for a polished exhibition presentation.

---

## Slide 1 — Title Slide

**Title:** Smart EV Charging Station with Battery Management System (BMS)

**Subtitle:** Arduino Nano | GSM SIM900A | UPI Payment | Battery Safety

**Institution / Team / Year**

**Visual:** EV charging icon + circuit board background

---

## Slide 2 — Problem Statement

**Heading:** Why Smart EV Charging?

**Bullets:**
- EV sales growing at 30% CAGR in India — charging infrastructure lagging
- Unmanaged charging degrades battery packs: ₹30K–₹80K in premature replacements
- No payment intelligence at small stations → cash-only, fraud risk
- Thermal runaway in Li-ion due to overcharging without temperature monitoring
- Commercial EVSE stations cost ₹1.5L–₹5L per port — unaffordable for colleges and apartments

**Visual:** Bar chart of EV growth vs. charging station count (illustrative)

---

## Slide 3 — Our Solution

**Heading:** Proposed Solution

**One-line pitch:** A smart, low-cost (₹2,625) EV charging controller that automatically verifies UPI payment via SMS and protects the battery with multi-layer safety.

**Solution pillars (icons):**
- 💳 UPI Payment via GSM SMS
- 🔋 BMS Battery Protection
- 🌡️ Temperature Monitoring
- ⚡ SSR Charging Control
- 📟 LCD Real-Time Feedback

---

## Slide 4 — System Architecture

**Heading:** System Architecture

**Visual:** Mermaid block diagram (exported from README.md)

**Key callouts:**
- Arduino Nano: Central brain (32KB flash, 2KB SRAM)
- GSM SIM900A: SMS payment verification
- BMS Module: Hardware-level battery protection
- SSR: Silent, arc-free charging control

---

## Slide 5 — Working Principle

**Heading:** How It Works — 5 Steps

1. User sends UPI payment to station SIM number
2. SIM900A receives SMS; Arduino parses payment confirmation
3. Pre-charge safety check: voltage + temperature within range?
4. SSR activates → charging begins; LCD updates every 2 seconds
5. Auto-stop on full charge (SOC ≥ 98%) or fault detection

**Visual:** Simplified flowchart (5-step linear flow)

---

## Slide 6 — Hardware Components

**Heading:** Hardware Used

**Table:**

| Component | Role |
|---|---|
| Arduino Nano | Controller |
| GSM SIM900A | Payment SMS |
| BMS Module (3S) | Battery Protection |
| SSR | Charging Control |
| LM35 | Temperature Sensor |
| LCD 16×2 | Display |
| Voltage Divider | Voltage Sensing |

**Visual:** Photograph of assembled prototype

---

## Slide 7 — Battery Safety Logic

**Heading:** Multi-Layer Battery Protection

**Diagram:** Battery safety flowchart (from README.md)

**Callouts:**
- Layer 1: BMS hardware IC — sub-millisecond response
- Layer 2: Software monitoring — every 2 seconds
- Protections: Overvoltage, Overtemperature, Overcurrent, Short Circuit

---

## Slide 8 — Live Demo

**Heading:** Live Demonstration

**Script:**
1. Show IDLE state on LCD
2. Send ₹50 UPI payment from phone to station number
3. Watch LCD update: "Payment OK → Charging Started"
4. Show SOC % rising on LCD
5. Trigger overtemperature (warm LM35 with hand) — show fault cutoff
6. Show system auto-recovery

**Have phone ready; ensure SIM card has network; test at least 30 min before presentation**

---

## Slide 9 — Results

**Heading:** Results & Observations

| Test | Outcome |
|---|---|
| SMS payment parsing | ~3 seconds from SMS to relay activation |
| Temperature protection | Triggered correctly at 45°C threshold |
| Voltage accuracy | ±0.1V across 9V–12.6V range |
| SSR switching | Silent, no arcing on every state transition |
| LCD updates | 2-second intervals throughout session |
| BMS protection | Prevented overcharging beyond 4.2V/cell |
| EEPROM logging | Session data persists across power cycles |

---

## Slide 10 — Future Scope

**Heading:** What's Next?

**V2 — ESP32 + Wi-Fi:** Real-time MQTT dashboard, RFID authentication, current sensing

**V3 — Cloud Connected:** Multi-station management, QR payment via API

**V4 — Mobile App:** Flutter app with session history and wallet

**V5 — AI Analytics:** Battery degradation prediction, adaptive charging

**V6 — Grid Integration:** OCPP compliance, solar MPPT charging

**Tagline:** *Scalable from college campus to commercial fleet depot.*

---

## Slide 11 — Thank You

**Heading:** Thank You

**Team members and roles**

**Guide / Mentor name**

**Institution name and department**

**GitHub Repository:** `github.com/prashanth-2369002/SmartEV-Charging-System`

**QR code linking to GitHub repository**

---

## Presentation Tips for Exhibition

- **Practice demo** with actual SMS send at least 5 times before the event
- **Carry a backup phone** with the UPI app pre-configured
- **Label all components** on the breadboard with sticky labels or a printed overlay
- **Bring a printed schematic** in case judges want to examine circuit design
- **Prepare for questions:** judges often ask about cost breakdown, safety certifications, and scalability
- **Know your numbers:** 32KB flash, 2KB SRAM, ₹2,625 total cost, 45°C cutoff, 4.2V/cell limit

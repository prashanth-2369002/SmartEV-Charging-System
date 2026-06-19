# LinkedIn Showcase Post

> Copy and paste this post to LinkedIn when sharing the project. Edit your name, college, and GitHub link before posting.

---

## Post Text

---

🚗⚡ Excited to share my Diploma Final Year Project — **Smart EV Charging Station with Battery Management System (BMS)** — selected and presented at a **State-Level Technical Exhibition**!

---

**The Problem:**
India's EV adoption is growing at 30% CAGR, but affordable smart charging infrastructure is still lagging — especially for college campuses, apartment parking, and rural setups. Commercial stations cost ₹1.5L–₹5L per port. Existing low-cost chargers have no payment intelligence and no battery safety.

**What I Built:**
A complete EV charging controller that solves this — for under ₹3,000 in hardware.

---

🔧 **Tech Stack:**
- **Arduino Nano (ATmega328P)** — central controller in 32KB flash, 2KB SRAM
- **GSM SIM900A** — receives UPI payment SMS confirmations via AT commands
- **Commercial BMS Module** — hardware-level overvoltage, overcurrent, and short-circuit protection
- **Solid State Relay (SSR)** — silent, arc-free charging circuit switching
- **LM35 Temperature Sensor** — real-time thermal monitoring with auto-cutoff
- **16×2 LCD** — live SOC%, voltage, temperature, and session timer display

---

⚙️ **Key Engineering Work:**
✅ Wrote C++ SMS parsing algorithm (in 32KB flash!) to verify UPI payment keywords from raw GSM AT responses
✅ Implemented multi-layer battery protection: BMS hardware cutoffs + software voltage/temperature monitoring
✅ Built voltage divider + ADC pipeline for accurate 3S Li-ion pack voltage reading
✅ Designed SSR-based charging control with proper state machine transitions
✅ Engineered non-blocking timer architecture using millis() — no delay() in main loop
✅ Implemented EEPROM ring-buffer session logger (up to 90 sessions, persistent across power cycles)

---

📊 **Results from Physical Prototype:**
- Payment SMS verified and relay activated in ~3 seconds
- Temperature protection triggered correctly at 45°C threshold
- Voltage reading accurate to ±0.1V across full 3S pack range
- SSR operated silently on every state transition — zero arcing
- Session data persisted across power cycles via EEPROM

---

🔭 **Future Scope (V2–V6):**
ESP32 + Wi-Fi → Cloud Dashboard → Flutter Mobile App → AI Battery Analytics → OCPP Grid Integration

---

🏆 **Selected for State-Level Technical Exhibition** — live demo with judges evaluating system under real UPI payment conditions.

📂 **Full repository** (firmware, docs, architecture diagrams, BMS design):
👉 https://github.com/prashanth-2369002/SmartEV-Charging-System

---

🔎 Open to **Embedded Systems**, **EV Technology**, and **IoT** internship opportunities!

Feel free to connect or drop a question in the comments.

---

**Hashtags:**
#EmbeddedSystems #ElectricVehicles #EVCharging #BatteryManagement #Arduino #GSM #IoT #Engineering #DiplomProject #SmartCharging #PowerElectronics #MadeInIndia #EngineeringStudent #TechForGood #SustainableTransportation

---

## Post Image Suggestions

Attach one of these for maximum engagement:

1. **Photo of prototype** — full breadboard setup, all components visible, LCD showing "Charging Started"
2. **Architecture diagram** — export the Mermaid block diagram from README.md as PNG
3. **Collage** — 4-panel: prototype overview + LCD close-up + phone showing SMS + state exhibition photo

**Recommended image size:** 1200×628 px (LinkedIn feed optimal)

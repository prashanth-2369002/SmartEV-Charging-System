# Images — Smart EV Charging Station with BMS

This directory contains visual assets for the project. Replace placeholder descriptions with actual photographs and diagrams from your physical prototype.

---

## Required Photographs (from physical prototype)

| Filename | Description | Status |
|---|---|---|
| `prototype_overview.jpg` | Full breadboard/PCB setup showing all components: Arduino Nano, SIM900A, BMS, SSR, LM35, LCD, voltage divider | To be added |
| `lcd_idle.jpg` | LCD showing "EV Charger Ready / Send UPI Payment" | To be added |
| `lcd_charging.jpg` | LCD showing live SOC %, voltage (e.g. "SOC:75% 11.6V / 30°C  05:32") | To be added |
| `lcd_fault.jpg` | LCD showing fault state (e.g. "! FAULT ! / Overtemperature") | To be added |
| `lcd_complete.jpg` | LCD showing session complete ("Session Complete / SOC:98% 22min") | To be added |
| `gsm_sim900a.jpg` | SIM900A module close-up with SIM card inserted and antenna visible | To be added |
| `bms_module.jpg` | 3S BMS protection module close-up showing cell connection pads and MOSFETs | To be added |
| `ssr_relay.jpg` | Solid State Relay with control wires and load connections labeled | To be added |
| `lm35_sensor.jpg` | LM35 temperature sensor on breadboard | To be added |
| `voltage_divider.jpg` | Resistor voltage divider circuit for battery voltage sensing | To be added |
| `payment_demo.jpg` | Phone screen showing UPI payment and confirmation SMS | To be added |
| `full_circuit.jpg` | Top-down view of complete circuit showing all wiring | To be added |
| `state_exhibition.jpg` | Project at state-level technical exhibition (judges/audience) | To be added |

---

## Diagrams Directory (`diagrams/`)

Mermaid diagrams from the README and docs can be exported to PNG/SVG:

| Filename | Source | Description |
|---|---|---|
| `block_diagram.png` | README.md — Block Diagram section | Hardware layer block diagram |
| `system_architecture.png` | README.md — System Architecture section | Full system architecture |
| `workflow.png` | README.md — Working Flow section | Complete flowchart |
| `gsm_flow.png` | README.md — GSM Communication Flow section | Sequence diagram |
| `battery_safety.png` | README.md — Battery Safety Logic section | Protection decision tree |
| `state_machine.png` | README.md — Charging State Machine section | State machine diagram |
| `bms_architecture.png` | docs/BMS_Design.md | BMS dual-layer architecture |
| `v2_roadmap_gantt.png` | docs/version_2_roadmap.md | Gantt chart |

### Exporting Mermaid Diagrams to PNG

Option 1 — **Mermaid Live Editor** (mermaid.live):
1. Paste the Mermaid code from README.md
2. Click "Actions" → "Download PNG" or "Download SVG"
3. Save to `images/diagrams/`

Option 2 — **GitHub native rendering**:
GitHub renders Mermaid in markdown files automatically. No export needed if linking to the markdown source.

Option 3 — **CLI tool**:
```bash
npm install -g @mermaid-js/mermaid-cli
mmdc -i diagram.mmd -o diagram.png -w 1200
```

---

## Image Naming Convention

- Use lowercase with underscores: `prototype_overview.jpg`
- Prefix with category: `lcd_`, `gsm_`, `bms_`, `circuit_`, `diagram_`
- Export photos at minimum 800×600 px
- Export diagrams at minimum 1200px width

---

## Caption Suggestions

When embedding images in the README, use descriptive captions:

```markdown
![Prototype Overview](images/prototype_overview.jpg)
*Complete prototype showing Arduino Nano controller (center), SIM900A GSM module (left), BMS module (back), and LCD display (top).*

![LCD Charging Display](images/lcd_charging.jpg)
*16×2 LCD displaying real-time SOC (75%), pack voltage (11.6V), temperature (30°C), and session time (05:32) during an active charging session.*
```

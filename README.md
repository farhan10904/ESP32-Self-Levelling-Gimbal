# ESP32 Self-Levelling Gimbal

ESP32-based two-axis self-levelling gimbal platform using an MPU6050 IMU, MG996R servos, PID control, a live WiFi dashboard, and a custom KiCad carrier PCB.

The project was built to understand embedded control systems, IMU sensor fusion, PID tuning, servo actuation, WiFi dashboards, PCB carrier-board design, and mechanical prototyping using Fusion 360 and 3D printed parts.

---

## Project Summary

This project controls a two-axis gimbal platform using live roll and pitch angle data from an MPU6050 IMU.

The ESP32 reads the IMU data, applies a complementary filter, calculates correction values using a PID control loop, and drives two MG996R servos to keep the platform level.

The system also includes a browser dashboard hosted by the ESP32 for:

- live roll and pitch monitoring
- manual servo control
- reset-to-flat control
- switching between stabilisation and dashboard control modes

A custom KiCad carrier PCB was designed after breadboard testing to make the wiring cleaner and allow the ESP32, servo connectors, power input, and remote IMU cable to plug into a permanent board while keeping the main modules reusable.

A custom Fusion 360 mechanical frame and electronics enclosure were also designed to mount the PCB, battery holder, wiring, servos, and moving platform into a more complete final build.

---

## Current Status

- [x] Wokwi simulation completed
- [x] MPU6050 angle reading implemented
- [x] Complementary filter implemented
- [x] PID control loop implemented
- [x] Roll and pitch servo control working
- [x] WiFi dashboard working
- [x] Manual dashboard control working
- [x] Reset-to-flat control working
- [x] Heartbeat safety system implemented
- [x] Real ESP32 breadboard test completed
- [x] Real servo and IMU behaviour tested
- [x] KiCad carrier PCB schematic completed
- [x] KiCad PCB layout routed
- [x] PCB DRC check passed
- [x] 3D PCB preview generated
- [x] PCB ordered
- [x] Fusion 360 mechanical frame designed
- [x] Electronics enclosure and removable lid designed
- [x] 3D print files exported for manufacturing
- [x] 3D printed parts submitted for manufacturing quote/order
- [ ] 3D printed parts received
- [ ] PCB received
- [ ] Final mechanical assembly
- [ ] Final PID tuning on printed frame
- [ ] Final demo video

---

## Demo / Design Preview

### Full Fusion 360 Assembly

<img src="docs/images/cad_full_assembly.png" alt="Fusion 360 full assembly of ESP32 self-levelling gimbal with electronics box" width="550">

### Electronics Box

<img src="docs/images/cad_electronics_box.png" alt="Fusion 360 electronics box for ESP32 PCB, battery holder, wiring and cable cutouts" width="500">

### Electronics Box Lid

<img src="docs/images/cad_electronics_lid.png" alt="Fusion 360 removable friction-fit lid for ESP32 gimbal electronics box" width="500">

### Roll Servo Bracket

<img src="docs/images/cad_roll_servo_bracket.png" alt="Fusion 360 roll servo bracket for ESP32 self-levelling gimbal" width="500">

### Base / Pitch Servo Bracket

<img src="docs/images/cad_base_pitch_servo_bracket.png" alt="Fusion 360 pitch servo bracket and base support for ESP32 self-levelling gimbal" width="500">

### Stabilising Platform

<img src="docs/images/cad_stabilizing_platform.png" alt="Fusion 360 moving stabilising platform with servo horn mounting holes" width="500">

### 3D PCB Preview

<img src="docs/images/kicad-pcb-preview.png" alt="3D preview of ESP32 self-levelling gimbal carrier PCB" width="500">

### PCB Routing Layout

<img src="docs/images/kicad-pcb-editor-close-up.png" alt="KiCad PCB routing layout for ESP32 gimbal carrier board" width="500">

### Initial Schematic Sheet

<img src="docs/images/ESP32 Intial Gimbal Schematic PCB.png" alt="KiCad initial schematic for ESP32 gimbal carrier board" width="650">

### Final Carrier Board Schematic

<img src="docs/images/kicad-schematic-close-up.png" alt="KiCad schematic for ESP32 gimbal carrier board" width="650">

### Full Schematic Sheet

<img src="docs/images/kicad-schematic-full.png" alt="Full KiCad schematic sheet for ESP32 gimbal PCB" width="650">

---

## Simulation Preview

The project was first built and tested in Wokwi before moving to real hardware.

The simulation was used to check:

- ESP32 code structure
- MPU6050 angle readings
- PID logic
- servo response
- WiFi dashboard layout
- manual dashboard control
- reset-to-flat behaviour

### Web Dashboard Simulation

<img src="docs/images/dashboard.png" alt="ESP32 self-levelling gimbal WiFi dashboard" width="700">

### Wokwi Circuit Simulation

<img src="docs/images/wokwi-circuit.png" alt="Wokwi circuit wiring layout for ESP32 gimbal" width="500">

---

## Features

- Two-axis roll and pitch stabilisation
- MPU6050 accelerometer and gyroscope angle sensing
- Complementary filter for smoother roll/pitch estimation
- PID control loop for real-time attitude correction
- MG996R servo control
- ESP32-hosted WiFi dashboard
- Live roll and pitch values in browser
- Manual dashboard control mode
- Reset-to-flat button
- Heartbeat safety system:
  - if the dashboard disconnects, the ESP32 returns to stabilisation mode
- Real breadboard hardware test
- Custom KiCad carrier PCB
- Remote MPU6050 connector so the IMU can be mounted on the moving platform
- 1000 µF bulk capacitor on the 5V servo rail
- Separate 5V servo power input
- Common ground between ESP32, servo supply, and IMU
- Custom Fusion 360 mechanical frame modifications
- 3D printable electronics enclosure for PCB, battery holder, boost converter and wiring
- Removable friction-fit electronics lid
- Cable cutouts for USB, servo wiring, switch wiring and remote MPU6050 cable
- M3 PCB mounting holes for screw/nut/washer spacing

---

## Hardware Used

| Component                            | Purpose                                        |
| ------------------------------------ | ---------------------------------------------- |
| ESP32 Dev Board                      | Main microcontroller and WiFi dashboard server |
| MPU6050 IMU                          | Measures roll and pitch angle                  |
| MG996R Servo x2                      | Controls roll and pitch axes                   |
| External 5V Supply / Boost Converter | Powers the servos                              |
| 2x 18650 Battery Holder              | Portable power source for the build            |
| 1000 µF Electrolytic Capacitor       | Reduces servo voltage dips                     |
| 100 nF Ceramic Capacitors            | Local power decoupling                         |
| Custom KiCad Carrier PCB             | Permanent wiring carrier board                 |
| Jumper / Servo Wires                 | Connects remote IMU and servos                 |
| 3D Printed Gimbal Frame              | Holds the roll and pitch servo mechanism       |
| 3D Printed Electronics Box           | Holds the PCB, battery holder and wiring       |
| 3D Printed Removable Lid             | Covers and protects the electronics bay        |
| M3 Screws / Nuts / Washers           | Mounts PCB and frame parts                     |
| M2 Self-Tapping Screws               | Fixes servo horn to moving platform            |

---

## Final Pin Connections

| Module      | Pin      | ESP32 / Power Connection            |
| ----------- | -------- | ----------------------------------- |
| MPU6050     | SDA      | GPIO 21                             |
| MPU6050     | SCL      | GPIO 22                             |
| MPU6050     | VCC      | ESP32 3.3V                          |
| MPU6050     | GND      | Common GND                          |
| Roll Servo  | Signal   | GPIO 13                             |
| Pitch Servo | Signal   | GPIO 19                             |
| Servos      | VCC      | External 5V rail                    |
| Servos      | GND      | External GND rail                   |
| ESP32       | VIN / 5V | 5V input rail if externally powered |
| ESP32       | GND      | Common GND                          |

> The MPU6050 is connected through a remote 4-pin connector so it can be mounted on the moving platform instead of fixed to the PCB.

Remote IMU connector order:

```text
SDA
SCL
GND
3V3
```

Servo connector order:

```text
SIG
5V
GND
```

---

## Power Setup

The servos are powered from a separate 5V rail because MG996R servos can draw much more current than the ESP32 can safely provide.

The ESP32, servo supply, and MPU6050 must share a common ground.

```text
External 5V supply → servo 5V rail
External GND       → servo GND rail
ESP32 GND          → common GND
MPU6050 VCC        → ESP32 3.3V
MPU6050 GND        → common GND
```

The carrier PCB includes:

- 5V power input connector
- 1000 µF capacitor across 5V and GND
- 100 nF capacitors on logic power rails
- 1 mm traces for 5V and GND power paths
- 0.4 mm trace for 3.3V
- 0.3 mm traces for signal lines

---

## Servo Power Warning

Do not power MG996R servos directly from:

```text
ESP32 3.3V pin
ESP32 USB 5V pin
```

A weak or noisy servo power supply can cause:

- servo jitter
- ESP32 brownouts
- WiFi dropouts
- unstable PID correction
- random resets
- inconsistent angle response

The 1000 µF capacitor helps absorb short servo current spikes, but it does not replace a proper 5V supply.

---

## Carrier PCB Design

The final carrier PCB was designed in KiCad to make the project cleaner and more reusable.

The board includes:

- ESP32 left/right socket headers
- roll servo header
- pitch servo header
- remote MPU6050 connector
- 5V power input
- 1000 µF servo rail capacitor
- 100 nF decoupling capacitors
- M3 mounting holes
- labelled silkscreen for all connectors
- wide 5V and GND traces for servo current

The ESP32 and MPU6050 are not permanently soldered to the PCB. The design uses sockets/connectors so the main modules can be removed and reused.

---

## Mechanical CAD and 3D Printed Frame

The mechanical design was completed in Fusion 360.

The original downloaded self-stabilising gimbal model was used as a starting point, but the electronics holder and frame layout were modified to fit the final ESP32 carrier PCB and make the build easier to assemble.

The final mechanical design uses:

- a two-axis roll/pitch gimbal frame
- MG996R-style servo mounting positions
- a custom electronics box mounted below the gimbal
- a removable friction-fit lid
- PCB mounting holes for M3 screws
- extra internal space for wiring slack
- cutouts for USB, servo wires, switch wiring and the remote MPU6050 cable
- a moving platform with servo horn mounting holes

The yaw-axis motor from the original model was removed because the project only needs roll and pitch self-levelling. This reduced unnecessary weight, removed an unused degree of freedom, and simplified the control problem.

The electronics enclosure was designed as a separate box rather than forcing all electronics into the original handle. This made the PCB, battery holder, boost converter and wiring easier to access and repair.

---

## 3D Printed Parts

The exported print parts are:

| Part                                       | Purpose                                                 |
| ------------------------------------------ | ------------------------------------------------------- |
| `Gimbal_Electronics_Handle_Box_Final.step` | Main electronics box for PCB, battery holder and wiring |
| `ESP32 Electrical Handle Lid Final.step`   | Removable friction-fit electronics lid                  |
| `Roll Servo Final.step`                    | Roll-axis servo bracket                                 |
| `Base Pitch servo Final.step`              | Base/pitch servo bracket                                |
| `Self-Stabilizing Platform Final.step`     | Moving platform/table section                           |

The parts were exported separately so they can be printed as independent pieces and assembled using screws, nuts, washers and servo hardware.

---

## Planned 3D Print Settings

The parts were prepared for external 3D printing using:

```text
Material: PLA
Process: FDM
Infill: 60%
Units: mm
Threads/tapped holes: No
Inserts: No
Assembly: No
```

PLA was chosen because it is cheap, strong enough for the demo, and suitable for functional prototype brackets and enclosures.

The design does not rely on printed threads. Mechanical fastening is handled with external screws, nuts and washers.

---

## Mechanical Assembly Plan

```text
Electronics box → supports the gimbal frame
PCB → mounted inside box using M3 screws, nuts and washers
Battery holder → mounted inside electronics box
Boost converter → mounted inside electronics box
MPU6050 → mounted remotely on moving platform
Servo horn → fixed to platform using small self-tapping screws
Lid → friction-fit removable cover
```

The PCB is intentionally kept removable. The ESP32 plugs into socket headers, the MPU6050 connects through a remote cable, and the servos connect through headers rather than being permanently soldered.

---

## Why the MPU6050 Is Remote

The MPU6050 must measure the angle of the moving platform.

If the IMU is fixed to the base or handle, it measures the wrong body and the PID loop corrects the wrong motion.

Final design:

```text
Main PCB → fixed to handle/base
MPU6050 → mounted on moving platform
Connection → 4-wire remote cable
```

The I2C wires should be kept reasonably short and routed away from servo power wires.

---

## WiFi Dashboard

The ESP32 connects to WiFi and serves a local browser dashboard.

For Wokwi simulation:

```cpp
const char* ssid = "Wokwi-GUEST";
const char* password = "";
```

For real hardware, update the WiFi credentials in `main.cpp`:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

Once connected, the ESP32 prints its IP address to the serial monitor.

Open that IP address in a browser to access the dashboard.

---

## Dashboard Modes

### Stabilise Mode

Default mode.

The ESP32 reads roll and pitch from the MPU6050, calculates the error from level, and uses PID output to move the servos.

```text
Target angle = 0°
Measured angle = MPU6050 filtered angle
Error = target angle - measured angle
```

### Dashboard Control Mode

Manual control mode.

The browser sliders set the target roll and pitch angles. The servos move to hold those target angles.

Useful for:

- checking servo direction
- testing dashboard control
- debugging PID response
- showing manual control in a demo

### Heartbeat Safety

The browser sends a heartbeat signal while the dashboard is open.

If the ESP32 stops receiving the heartbeat, it automatically returns to stabilise mode.

This prevents the system from staying stuck in manual control mode after the browser closes.

---

## PID Control

Current baseline PID values:

| Axis  |  Kp |  Ki |  Kd |
| ----- | --: | --: | --: |
| Roll  | 2.5 | 0.0 | 0.4 |
| Pitch | 2.5 | 0.0 | 0.4 |

These values worked as a starting point but may need adjustment once the final 3D printed frame is assembled.

Recommended tuning process:

1. Set `Ki = 0` and `Kd = 0`.
2. Increase `Kp` until the platform corrects quickly.
3. If it oscillates, reduce `Kp`.
4. Add `Kd` to reduce overshoot.
5. Only add `Ki` if there is a steady offset that does not disappear.

---

## How It Works

The MPU6050 provides accelerometer and gyroscope data.

The accelerometer gives a gravity-based angle estimate but is noisy during movement.

The gyroscope gives smooth short-term angle changes but drifts over time.

A complementary filter combines both:

```text
filtered angle = gyro short-term estimate + accelerometer long-term correction
```

The ESP32 then compares the filtered angle with the target angle:

```text
error = target angle - measured angle
```

The PID controller calculates the correction:

```text
PID output = proportional + integral + derivative correction
```

The correction is converted into servo angle commands:

```text
MPU6050 → complementary filter → PID → servo output → platform correction
```

---

## Problems Faced and Fixes

| Problem                                                         | Fix                                                             |
| --------------------------------------------------------------- | --------------------------------------------------------------- |
| Dashboard layout was too wide                                   | Reworked CSS layout and container sizing                        |
| Manual sliders needed cleaner control                           | Added dashboard control mode                                    |
| Browser disconnect could leave system in manual mode            | Added heartbeat safety system                                   |
| GPIO12 was originally used for pitch servo                      | Changed pitch servo to GPIO19 to avoid ESP32 boot issues        |
| Servos caused high current demand                               | Added separate 5V servo rail and 1000 µF capacitor              |
| MPU6050 needed to move with the platform                        | Changed PCB design to use a remote IMU connector                |
| ESP32 module needed to stay reusable                            | Used 2x 1x15 socket headers                                     |
| PCB mounting needed to fit a printed frame                      | Added M3 mounting holes                                         |
| Original gimbal model included an unnecessary yaw motor         | Removed the yaw-axis design and kept the system two-axis only   |
| Original handle did not fit the PCB, battery and wiring cleanly | Designed a new electronics box and removable lid in Fusion 360  |
| Servo horn mounting holes were not suitable for large screws    | Planned smaller self-tapping screws for horn-to-platform fixing |
| Electronics needed to remain serviceable                        | Used a removable lid and accessible PCB/battery layout          |

---

## Software

Built with PlatformIO in VS Code using the Arduino framework.

### `platformio.ini`

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    electroniccats/MPU6050
    madhephaestus/ESP32Servo
```

---

## How to Run

1. Clone the repo.
2. Open the project in VS Code.
3. Install the PlatformIO extension.
4. Connect the ESP32 by USB.
5. Build the project.
6. Upload it to the ESP32.
7. Open the serial monitor at `115200` baud.
8. Wait for the ESP32 to print its IP address.
9. Open that IP address in a browser.

---

## Wokwi Simulation

1. Open the project in VS Code.
2. Build the PlatformIO project.
3. Open `diagram.json`.
4. Run the Wokwi simulation.
5. Open the ESP32 dashboard URL shown in the serial monitor.

---

## Project Pipeline

- [x] Wokwi simulation
- [x] MPU6050 angle reading
- [x] Complementary filter
- [x] PID control loop
- [x] Servo control
- [x] WiFi dashboard
- [x] Manual dashboard mode
- [x] Dashboard safety heartbeat
- [x] Real breadboard hardware test
- [x] KiCad schematic
- [x] KiCad carrier PCB layout
- [x] PCB DRC check
- [x] PCB 3D preview
- [x] Upload final KiCad files
- [x] Order PCB
- [x] Design 3D printed frame in Fusion 360
- [x] Design electronics box and removable lid
- [x] Export 3D print files
- [x] Submit 3D print files for manufacturing
- [ ] Receive PCB
- [ ] Receive 3D printed parts
- [ ] Mount PCB to electronics box
- [ ] Mount MPU6050 to moving platform
- [ ] Final mechanical assembly
- [ ] Final PID tuning
- [ ] Final demo video
- [ ] Portfolio write-up

---

## Planned Demo Video

The final demo video will show:

- gimbal correcting roll and pitch movement
- dashboard showing live angle values
- manual dashboard control
- reset-to-flat behaviour
- PCB mounted inside/onto the gimbal frame
- remote MPU6050 mounted on the moving platform
- electronics box and removable lid
- final 3D printed frame assembly

---

## Repository Structure

```text
ESP32-Self-Levelling-Gimbal/
├── cad/
│   └── exported-print-files/
│       ├── Base Pitch servo Final.step
│       ├── ESP32 Electrical Handle Lid Final.step
│       ├── Gimbal_Electronics_Handle_Box_Final.step
│       ├── Roll Servo Final.step
│       └── Self-Stabilizing Platform Final.step
├── docs/
│   └── images/
│       ├── dashboard.png
│       ├── wokwi-circuit.png
│       ├── kicad-pcb-preview.png
│       ├── kicad-schematic-close-up.png
│       ├── kicad-schematic-full.png
│       ├── kicad-pcb-editor-close-up.png
│       ├── cad_full_assembly.png
│       ├── cad_electronics_box.png
│       ├── cad_electronics_lid.png
│       ├── cad_roll_servo_bracket.png
│       ├── cad_base_pitch_servo_bracket.png
│       └── cad_stabilizing_platform.png
├── include/
├── lib/
├── src/
│   └── main.cpp
├── test/
├── diagram.json
├── platformio.ini
├── wokwi.toml
└── README.md
```

---

## Author

**Farhan Ali**  
Engineering Student / Embedded Systems Project

- GitHub: [farhan10904](https://github.com/farhan10904)
- Portfolio: [Farhan Ali Engineering Portfolio](https://pacific-attention-6cd.notion.site/Farhan-Ali-Engineering-Portfolio-2c0495dbdc658028a0decf9447459ea6#367495dbdc65808eb791f741fc051231)
- LinkedIn: [Farhan Ali](https://www.linkedin.com/in/farhan-ali-95047a245/)

Built as an independent engineering project to practise embedded systems, control systems, PID tuning, IMU sensor fusion, PCB design, and mechanical prototyping.

---

## Acknowledgements

The original mechanical gimbal bracket and stabilising platform geometry were based on the DIY Arduino Gimbal project by HowToMechatronics: [DIY Arduino Gimbal | Self-Stabilizing Platform](https://howtomechatronics.com/projects/diy-arduino-gimbal-self-stabilizing-platform/)

I adapted the mechanical design for my own ESP32-based version, including changes to the electronics enclosure, PCB integration, wiring layout, power system, Wi-Fi dashboard, and control implementation.

AI tools were used for planning support, debugging help, code review, and documentation improvements.

The hardware choices, circuit changes, PID testing, PCB design decisions, CAD changes, mechanical layout decisions, and final project direction were reviewed, tested, and understood by the author.

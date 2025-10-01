# ESP32 ↔ Pico 2 W SPP Angle Tracker

## Overview
This project links an **ESP32** and a **Raspberry Pi Pico 2 W** using **Classic Bluetooth SPP** to control four servos with four potentiometers.

- **ESP32 (SPP master/client):**
  - Reads 4 potentiometers on GPIO 32, 33, 34, 35.
  - Normalizes values with **rolling min/max**, **EMA smoothing (α=0.3)**, and **averaging (N=4)**.
  - Maps values to **angles (0–180°)**.
  - Sends data via Classic Bluetooth in format:  
    ```
    ANG:a,b,c,d
    ```
  - Prints `ang0..ang3` to Arduino Serial Plotter.

- **Pico 2 W (SPP server):**
  - Receives `ANG:…` from ESP32.
  - Drives 4 servos on GP2, GP3, GP4, GP5.
  - Applies ±2° **dead-band filter** to reduce jitter.
  - Prints `a0..a3` (actual servo angles) to Serial Plotter.
  - Blinks onboard LED: **fast = receiving data**, **slow = idle**.

---

## Repository Layout
ESP32_SPP_SendAngles/ # Arduino sketch for ESP32
└── ESP32_SPP_SendAngles.ino

Pico2W_SPP_Servos_AnglesOnly/ # Arduino sketch for Pico 2 W
└── Pico2W_SPP_Servos_AnglesOnly.ino

README.md
LICENSE

yaml
Copy code

---

## Hardware Setup

### ASCII Wiring Diagram

arduino
Copy code
     ┌───────────────┐                     ┌───────────────┐
     │     ESP32     │                     │   Pico 2 W    │
     │   (Client)    │                     │   (Server)    │
     │               │                     │               │
3V3 ──┼── Pot VCC 5V ── Servo VCC (external PSU)
GND ──┼── Pot GND ────────┐ Common GND ──┐
GPIO32 ─┼── Pot 1 wiper │ │
GPIO33 ─┼── Pot 2 wiper │ │
GPIO34 ─┼── Pot 3 wiper │ │
GPIO35 ─┼── Pot 4 wiper │ │
│ │ │
│ Bluetooth SPP │<───────►───────────────┘
│ sends ANG:a,b,c,d GP2 ── Servo 1 (signal)
│ GP3 ── Servo 2 (signal)
│ GP4 ── Servo 3 (signal)
│ GP5 ── Servo 4 (signal)
└───────────────┘ └───────────────┘

markdown
Copy code

**Notes:**
- ESP32 pots powered from **3.3 V**.  
- Servos powered from **external 5 V supply** (not Pico 3.3 V).  
- **Common ground** required between ESP32, Pico, and servo PSU.  
- Classic Bluetooth SPP link carries `ANG:0–180` values.

---

## Arduino IDE Settings

### ESP32
- **Board:** Your ESP32 dev board  
- **Baud rate:** 115200  
- **Libraries:** `BluetoothSerial` (comes with ESP32 core)

### Pico 2 W
- **Board:** Raspberry Pi **Pico 2 W**  
- **Tools → IP/Bluetooth Stack:** **Bluetooth** (or **IPv4 + Bluetooth** if Wi-Fi is needed)  
- **Baud rate:** 115200  
- **Libraries:** `SerialBT` (in RP2040 core), `Servo`

---

## Serial Plotter Output

- **ESP32:**  
  Prints `ang0..ang3` (angles computed from pots).  

- **Pico 2 W:**  
  Prints `a0..a3` (angles actually sent to servos).  

Open **Tools → Serial Plotter**, set **baud = 115200**, and see angles update in real time.

---

## How to Run

1. Flash **Pico 2 W** sketch first — it will advertise as `PICO2W_SPP`.  
2. Flash **ESP32** sketch — it connects via MAC or name.  
3. Open **Serial Plotter** on either board.  
4. Twist pots → servos move smoothly.  

---

## License
MIT License – see [LICENSE](LICENSE).

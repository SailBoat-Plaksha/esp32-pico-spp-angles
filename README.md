# ESP32 ↔ Pico 2 W SPP Angle Tracker

---

## Hardware Setup

### ESP32
- **Pots:**  
  - 3.3 V → Pot VCC  
  - GND → Pot GND  
  - Wipers → GPIO **32, 33, 34, 35**

### Pico 2 W
- **Servos:**  
  - Signals → GP2, GP3, GP4, GP5  
  - Power from **external 5 V supply** (not Pico’s 3.3 V!)  
  - **Common GND** between servo PSU, Pico, and ESP32

---

## Arduino IDE Settings

### For ESP32
- **Board:** Your ESP32 dev board  
- **Upload speed:** 115200 baud  
- **Libraries:** `BluetoothSerial` (included in ESP32 core)

### For Pico 2 W
- **Board:** Raspberry Pi **Pico 2 W**  
- **Tools → IP/Bluetooth Stack:** **Bluetooth** (or **IPv4 + Bluetooth** if Wi-Fi is needed)  
- **Libraries:** `SerialBT` (included in Pico RP2040 core), `Servo`  

---

## Serial Plotter Output

- **ESP32:**  
  Prints `ang0..ang3` (the mapped angles sent over Bluetooth).  

- **Pico 2 W:**  
  Prints `a0..a3` (the actual servo angles being written).  

👉 Open **Tools → Serial Plotter**, set **baud = 115200**, and watch the angles in real time.

---

## Wiring Diagram

![Wiring diagram](docs/wiring.svg)

> Pots take **3.3 V** from ESP32; servos take **5 V** from an external PSU.  
> **Common ground** between ESP32, Pico 2 W, and the servo PSU is required.  
> Bluetooth transport is **Classic SPP**: ESP32 sends `ANG:a,b,c,d`.


## How to Run

1. Flash the **Pico 2 W** sketch first. It advertises as `PICO2W_SPP`.  
2. Flash the **ESP32** sketch. It will connect via MAC or by name.  
3. Open Serial Plotter on both boards to visualize.  
4. Rotate pots → servos follow smoothly with dead-band filtering.  

---

## License
This project is licensed under the MIT License — see [LICENSE](LICENSE) for details.

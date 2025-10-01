# ESP32 ↔ Pico 2 W SPP Angle Tracker

## Overview
This project uses **Classic Bluetooth SPP** to link an **ESP32** and a **Raspberry Pi Pico 2 W**:

- **ESP32 (client/master):**
  - Reads 4 potentiometers (GPIO 32–35).
  - Normalizes, smooths, and maps values to 0–180°.
  - Sends formatted strings: `ANG:a,b,c,d`.
  - Prints angles for Serial Plotter.

- **Pico 2 W (server):**
  - Receives angle data over SPP.
  - Drives 4 servos (GP2–GP5).
  - Applies dead-band to reduce jitter.
  - Prints actual servo angles for Serial Plotter.
  - Onboard LED blinks fast when receiving data, slow when idle.

---

## Setup

### Hardware
- ESP32 with 4 potentiometers wired to GPIO 32–35.  
- Pico 2 W with 4 servos on pins GP2–GP5.  
- Servos powered by **external 5 V supply**.  
- **Common ground** required between ESP32, Pico, and servo PSU.

### Arduino IDE
- **ESP32:** Board = ESP32 dev board, baud = 115200, library = `BluetoothSerial`.  
- **Pico 2 W:** Board = Pico 2 W, Tools → *IP/Bluetooth Stack* = **Bluetooth**, baud = 115200, libraries = `SerialBT`, `Servo`.  

---

## Usage
1. Upload Pico sketch → it advertises as `PICO2W_SPP`.  
2. Upload ESP32 sketch → it connects by MAC or name.  
3. Open **Serial Plotter (115200 baud)** to view angles.  
4. Rotate pots → servos move accordingly.  

---

## License
MIT License – see [LICENSE](LICENSE).

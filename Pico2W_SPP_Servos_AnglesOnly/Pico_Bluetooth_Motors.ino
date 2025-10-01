// ---------------------- PICO 2 W  (SPP Server, no PIN) ----------------------
#include <SerialBT.h>
#include <Servo.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 25
#endif

// 4 PWM-capable pins for servos
const int SERVO_PINS[4] = {2, 3, 4, 5};
Servo servos[4];

// Dead-band to reduce jitter (keep if you like the smoothness)
const int DEAD_BAND_DEG = 2;
int lastAngle[4] = {90,90,90,90};

String btLine;
unsigned long lastSeenMs = 0;

static inline int clamp180(int x){ if(x<0)return 0; if(x>180)return 180; return x; }
static inline bool needsMove(int n, int p){ return abs(n - p) >= DEAD_BAND_DEG; }

static void writeWithDeadband(int idx, int target) {
  target = clamp180(target);
  if (needsMove(target, lastAngle[idx])) {
    servos[idx].write(target);
    lastAngle[idx] = target;
  }
}

static void plotAngles() {
  Serial.print("a0:"); Serial.print(lastAngle[0]); Serial.print('\t');
  Serial.print("a1:"); Serial.print(lastAngle[1]); Serial.print('\t');
  Serial.print("a2:"); Serial.print(lastAngle[2]); Serial.print('\t');
  Serial.print("a3:"); Serial.println(lastAngle[3]);
}

static void handleLine(String line) {
  line.trim(); if (line.isEmpty()) return;
  lastSeenMs = millis();

  if (line.startsWith("ANG:")) {
    int ang[4] = {lastAngle[0], lastAngle[1], lastAngle[2], lastAngle[3]};
    int idx = 0, last = 4;
    for (int i = 4; i <= (int)line.length(); ++i) {
      if (i == (int)line.length() || line[i] == ',') {
        if (idx < 4) ang[idx++] = line.substring(last, i).toInt();
        last = i + 1;
      }
    }
    if (idx != 4) return; // malformed line, ignore

    for (int i=0;i<4;i++) writeWithDeadband(i, ang[i]);
    plotAngles(); // angles only
  }
}

static bool isAlive(){ return (millis() - lastSeenMs) <= 1200; }

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  while (!Serial) { delay(10); }
  Serial.println("a0:\ta1:\ta2:\ta3:"); // Plotter header (angles only)

  for (int i=0;i<4;i++) {
    // If needed: servos[i].attach(SERVO_PINS[i], 500, 2500);
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(lastAngle[i]);
  }

  SerialBT.setName("PICO2W_SPP");
  SerialBT.begin(); // Classic BT SPP server (no PIN)
  Serial.println("Pico2W SPP ready. Advertising as 'PICO2W_SPP'.");
}

void loop() {
  while (SerialBT.available()) {
    char c = (char)SerialBT.read();
    if (c == '\n') {
      String line = btLine; btLine = "";
      handleLine(line);
    } else if (c != '\r') {
      if (btLine.length() < 255) btLine += c;
    }
  }

  // Heartbeat LED (fast when receiving, slow when idle)
  static unsigned long tBlink=0; static bool led=false;
  if (millis() - tBlink > (isAlive() ? 150 : 700)) {
    tBlink = millis();
    led = !led;
    digitalWrite(LED_BUILTIN, led);
  }
}

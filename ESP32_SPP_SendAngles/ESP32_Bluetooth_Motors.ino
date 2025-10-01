// -------- ESP32 (SPP Master/Client) — sends angles 0–180 & plots only angles ---
#include <BluetoothSerial.h>
#include "esp_log.h"

BluetoothSerial BT;

// ==== Target Pico-2W SPP name / MAC ====
const char* TARGET_NAME = "PICO2W_SPP";
// Replace with your Pico 2 W's MAC if you have it (faster & robust)
uint8_t PICO_MAC[6] = {0x2C, 0xCF, 0x67, 0xD1, 0xE3, 0x46}; // <— change if needed

// ==== Pots & normalization (same math as your monitor) ====
const int PINS_ADC[4] = {32, 33, 34, 35};  // adjust to your wiring
const int NADC = 4;
const int AVG_SAMPLES = 4;
const float EMA_ALPHA = 0.3f;
const uint16_t SEND_MS = 50;               // ~20 Hz

int  rawMin[4], rawMax[4];
bool inited[4] = {false,false,false,false};
float emaNormPct[4] = {0,0,0,0};

unsigned long lastSend = 0;

static inline float clampf(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

static int readAveragedADC(int pin) {
  long acc = 0;
  for (int i=0; i<AVG_SAMPLES; ++i) acc += analogRead(pin);
  return (int)(acc / AVG_SAMPLES);
}

static void resetCalibration() {
  for (int i=0; i<NADC; ++i) { inited[i] = false; emaNormPct[i] = 0.0f; }
  Serial.println("[ESP32] Calibration reset.");
}

static bool tryConnectByMac() {
  Serial.printf("Connecting to MAC %02X:%02X:%02X:%02X:%02X:%02X ...\n",
    PICO_MAC[0],PICO_MAC[1],PICO_MAC[2],PICO_MAC[3],PICO_MAC[4],PICO_MAC[5]);
  if (BT.connect(PICO_MAC)) { Serial.println("Connected by MAC."); return true; }
  Serial.println("MAC connect failed."); return false;
}

static bool tryConnectByName() {
  Serial.printf("Connecting to name '%s' ...\n", TARGET_NAME);
  if (BT.connect(TARGET_NAME)) { Serial.println("Connected by name."); return true; }
  Serial.println("Name connect failed."); return false;
}

void setup() {
  Serial.begin(115200);
  delay(200);
  esp_log_level_set("*", ESP_LOG_ERROR);

  for (int i=0; i<NADC; ++i) pinMode(PINS_ADC[i], INPUT);
  analogReadResolution(12); // 0..4095

  BT.begin("ESP32_MASTER", true);   // master/client
  Serial.println("ESP32 SPP master started.");
  if (!tryConnectByMac()) (void)tryConnectByName();

  // Serial Plotter header — angles only
  Serial.println("ang0:\tang1:\tang2:\tang3:");
}

void loop() {
  // Optional: press 'C' in Serial Monitor to re-calibrate
  while (Serial.available()) {
    int ch = Serial.read();
    if (ch=='C' || ch=='c') resetCalibration();
  }

  if (!BT.connected()) {
    static unsigned long lastRetry=0, retryDelay=1000; const unsigned long cap=8000;
    if (millis()-lastRetry >= retryDelay) {
      lastRetry = millis();
      if (!tryConnectByMac() && !tryConnectByName()) retryDelay = min(retryDelay*2, cap);
      else retryDelay = 500;
    }
    delay(10);
    return;
  }

  if (millis() - lastSend >= SEND_MS) {
    lastSend = millis();

    uint8_t angles[4];
    for (int i=0; i<NADC; ++i) {
      int raw = readAveragedADC(PINS_ADC[i]);

      if (!inited[i]) { rawMin[i]=rawMax[i]=raw; inited[i]=true; }
      else { if (raw<rawMin[i]) rawMin[i]=raw; if (raw>rawMax[i]) rawMax[i]=raw; }

      float n = 0.0f;
      int range = rawMax[i] - rawMin[i];
      if (range > 4) {
        n = ((float)(raw - rawMin[i]) / (float)range) * 100.0f;
        n = clampf(n, 0.0f, 100.0f);
      }
      emaNormPct[i] = EMA_ALPHA*n + (1.0f-EMA_ALPHA)*emaNormPct[i];

      int a = (int)lroundf(emaNormPct[i] * 1.80f); // 0..100% → 0..180
      if (a<0) a=0; if (a>180) a=180;
      angles[i] = (uint8_t)a;
    }

    // Send to Pico
    BT.printf("ANG:%d,%d,%d,%d\n", angles[0], angles[1], angles[2], angles[3]);

    // Plot angles only (for motors 0..3)
    Serial.print("ang0:"); Serial.print((int)angles[0]); Serial.print('\t');
    Serial.print("ang1:"); Serial.print((int)angles[1]); Serial.print('\t');
    Serial.print("ang2:"); Serial.print((int)angles[2]); Serial.print('\t');
    Serial.print("ang3:"); Serial.println((int)angles[3]);
  }
}

#include "MAX30105.h"
#include "heartRate.h"
#include "my_structs.h"
#include <LiquidCrystal_I2C.h>

extern SemaphoreHandle_t xSemaphoreMutex;
extern QueueHandle_t xAllSignalsDataQueue;
boolean isMAX30102Completed = false;

LiquidCrystal_I2C lcd(0x3F, 16, 2);
MAX30105 particleSensor;

void max30102_setup() {
  // Initialize sensor
  if (particleSensor.begin() == false) {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1)
      ;
  }

  particleSensor.setup();

  lcd.begin();
  lcd.backlight();
}

float *readSignalOfMax30102(int fs, float t) {
  int n = (int) fs * t;
  float *data = new float[n];
  int delayTime = 1000 / fs;

  for (int i = 0; i < n; i++) {
    data[i] = particleSensor.getIR();

    if (data[i] <= 50000) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NO FINGER...");
      esp_restart();
    }

    delay(delayTime);
  }

  return data;
}

void max30102_task(void *pvParameters) {
  (void)pvParameters;
  max30102_setup();

  while (true) {
    if (particleSensor.getIR() <= 50000) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NO FINGER...");
      esp_restart();
    }

    if (!isMAX30102Completed) {
      Serial.println("Task MAX3330102 đã hoạt động từ lệnh gọi (choice 1)!");
      float *data = readSignalOfMax30102(50, 20.48);
      AllSignalsData allSignalsData;

      xQueueReceive(xAllSignalsDataQueue, &allSignalsData, portMAX_DELAY);

      if (xSemaphoreTake(xSemaphoreMutex, portMAX_DELAY) == pdTRUE) {
        allSignalsData.ppgSignal = data;

        if (xQueueSend(xAllSignalsDataQueue, &allSignalsData, 0) == pdTRUE) {
          xSemaphoreGive(xSemaphoreMutex);
        }
      }

      isMAX30102Completed = true;
    }

    delay(10);
  }
}

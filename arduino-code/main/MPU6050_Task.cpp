#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "my_structs.h"

extern SemaphoreHandle_t xSemaphoreMutex;
extern QueueHandle_t xAllSignalsDataQueue;
boolean isMPU6050Completed = false;

Adafruit_MPU6050 mpu;

void mpu6050_setup() {
  while (!Serial) {
    delay(10);  // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);
}

MPU6050_Signals readSignalOfMPU6050(int fs, float t) {
  int n = (int) fs * t;
  MPU6050_Signals data;
  data.accelXSignals = new float[n];
  data.accelYSignals = new float[n];
  data.accelZSignals = new float[n];
  data.gyroXSignals = new float[n];
  data.gyroYSignals = new float[n];
  data.gyroZSignals = new float[n];
  int delayTime = 1000 / fs;

  for (int i = 0; i < n; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    data.accelXSignals[i] = a.acceleration.x;
    data.accelYSignals[i] = a.acceleration.y;
    data.accelZSignals[i] = a.acceleration.z;
    data.gyroXSignals[i] = g.gyro.x;
    data.gyroYSignals[i] = g.gyro.y;
    data.gyroZSignals[i] = g.gyro.z;

    delay(delayTime);
  }

  return data;
}

void mpu6050_task(void *pvParameters) {
  (void)pvParameters;
  mpu6050_setup();
  while (true) {
    if (!isMPU6050Completed) {
      Serial.println("Task MPU6050 đã hoạt động từ lệnh gọi (choice 1)!");
      MPU6050_Signals data = readSignalOfMPU6050(50, 20.48);

      if (xSemaphoreTake(xSemaphoreMutex, portMAX_DELAY) == pdTRUE) {
        AllSignalsData allSignalsData;
        allSignalsData.mpu6050Signals = data;

        if (xQueueSend(xAllSignalsDataQueue, &allSignalsData, portMAX_DELAY) == pdTRUE) {
          xSemaphoreGive(xSemaphoreMutex);
        }
      }

      isMPU6050Completed = true;
    }

    delay(10);
  }
}

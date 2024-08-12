#include <WiFi.h>
#include "MPU6050_Task.h"
#include "MAX30102_Task.h"
#include "WS_Task.h"
#include "my_structs.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

const char* ssid = "Hem 43";
const char* password = "qwertyuiop";

QueueHandle_t xMPU6050SignalsDataQueue;
QueueHandle_t xAllSignalsDataQueue;

SemaphoreHandle_t xSemaphoreMutex;

void setup() {
  Serial.begin(115200);

  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
  }

  // Check if connected to wifi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No Wifi!");
    return;
  }

  Serial.println("Connected to Wifi, Connecting to server.");

  xMPU6050SignalsDataQueue = xQueueCreate(1, sizeof(MPU6050_Signals));
  xAllSignalsDataQueue = xQueueCreate(1, sizeof(AllSignalsData));

  xSemaphoreMutex = xSemaphoreCreateMutex();

  xTaskCreate(
    ws_task, "Task WS", 2048, NULL, 1, NULL);

  xTaskCreate(
    mpu6050_task, "Task MPU6050", 2048, NULL, 2, NULL);

  xTaskCreatePinnedToCore(
    max30102_task, "Task MAX30102", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);

  vTaskStartScheduler();
}

void loop() {
}

#include "MPU6050_Task.h"
#include "MAX30102_Task.h"
#include "Predict_Task.h"
#include "my_structs.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

QueueHandle_t xAllSignalsDataQueue;
SemaphoreHandle_t xSemaphoreMutex;

void setup() {
  Serial.begin(115200);

  xAllSignalsDataQueue = xQueueCreate(1, sizeof(AllSignalsData));
  xSemaphoreMutex = xSemaphoreCreateMutex();

  xTaskCreate(
    predict_task, "Task predict", 2048, NULL, 1, NULL);

  xTaskCreate(
    mpu6050_task, "Task MPU6050", 2048, NULL, 2, NULL);

  xTaskCreatePinnedToCore(
    max30102_task, "Task MAX30102", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);

  vTaskStartScheduler();
}

void loop() {
}

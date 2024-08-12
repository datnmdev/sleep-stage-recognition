#include "MAX30105.h"
#include "heartRate.h"
#include "my_structs.h"

extern SemaphoreHandle_t xSemaphoreMutex;
extern QueueHandle_t xAllSignalsDataQueue;
extern boolean start;
extern int choice;
boolean isMAX30102Completed = false;

MAX30105 particleSensor;

void max30102_setup() {
  // Initialize sensor
  if (particleSensor.begin() == false) {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1)
      ;
  }

  particleSensor.setup();
}

float *readSignalOfMax30102(int fs, float t) {
  int n = int(fs * t);
  float *data = new float[n];
  int delayTime = 1000 / fs;

  for (int i = 0; i < n; i++) {
    data[i] = particleSensor.getIR();
    delay(delayTime);
  }

  return data;
}

String convertToString1(float *data, int n) {
  // Tạo một biến String để lưu trữ kết quả
  String str;

  // Sử dụng vòng lặp để duyệt qua từng phần tử trong mảng data
  for (int i = 0; i < n; i++) {
    // Chuyển đổi phần tử hiện tại sang chuỗi và nối vào str
    str += String(data[i]);

    // Nối thêm dấu cách vào str
    str += " ";
  }

  // Trả về chuỗi kết quả
  return str;
}

void max30102_task(void *pvParameters) {
  (void)pvParameters;
  max30102_setup();

  while (true) {
    if (start & !isMAX30102Completed) {
      switch (choice) {
        case 1:
          {
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
            break;
          }

        default:
          break;
      }
    }

    delay(10);
  }
}

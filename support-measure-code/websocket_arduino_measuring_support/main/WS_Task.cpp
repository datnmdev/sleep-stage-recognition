#include "esp_system.h"
#include <ArduinoWebsockets.h>
#include <Arduino_JSON.h>
#include <assert.h>
#include "my_structs.h"

using namespace websockets;

extern SemaphoreHandle_t xSemaphoreMutex;
extern QueueHandle_t xMPU6050SignalsDataQueue;
extern QueueHandle_t xAllSignalsDataQueue;
extern boolean isMPU6050Completed;
extern boolean isMAX30102Completed;
boolean start = false;
int choice = 0;

const char* websockets_server_host = "192.168.1.8";
const uint16_t websockets_server_port = 8080;

WebsocketsClient client;

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionClosed) {
    esp_restart();
  }
}

void releaseMPU6050SignalsMemory(MPU6050_Signals& data) {
  delete[] data.accelXSignals;
  delete[] data.accelYSignals;
  delete[] data.accelZSignals;
  delete[] data.gyroXSignals;
  delete[] data.gyroYSignals;
  delete[] data.gyroZSignals;
}

void releaseAllSignalsDataMemory(AllSignalsData& data) {
  delete[] data.mpu6050Signals.accelXSignals;
  delete[] data.mpu6050Signals.accelYSignals;
  delete[] data.mpu6050Signals.accelZSignals;
  delete[] data.mpu6050Signals.gyroXSignals;
  delete[] data.mpu6050Signals.gyroYSignals;
  delete[] data.mpu6050Signals.gyroZSignals;
  delete[] data.ppgSignal;
}

void ws_setup() {
  // try to connect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connected) {
    Serial.println("Connected!");
    client.send("{\"header\":\"CONNECT_DEVICES_TO_WS_SERVER\",\"data\":true}");
  } else {
    Serial.println("Not Connected!");
    esp_restart();
  }

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) {
    JSONVar obj = JSON.parse(message.data());

    if (JSON.typeof(obj) != "undefined") {
      String header = obj["header"];
      if (header.equals("MEASUREMENT_CMD")) {
        choice = (int)obj["choice"];
        start = true;
      } else if (header.equals("RESTART_CMD")) {
        esp_restart();
      }
    }
  });

  client.onEvent(onEventsCallback);

  // start = true;
}

String convertToString(float* data, int n) {
  // Tạo một biến String để lưu trữ kết quả
  String str;

  // Sử dụng vòng lặp để duyệt qua từng phần tử trong mảng data
  for (int i = 0; i < n; i++) {
    // Chuyển đổi phần tử hiện tại sang chuỗi và nối vào str
    str += String(data[i]);

    // Nối thêm dấu cách vào str
    str += ",";
  }

  // Trả về chuỗi kết quả
  return str;
}

void ws_task(void* pvParameters) {
  (void)pvParameters;
  ws_setup();

  while (true) {
    // let the websockets client check for incoming messages
    if (client.available()) {
      client.poll();
    }

    // Send signals data to ws server
    if (start) {
      if (choice == 0) {
        if (isMPU6050Completed) {
          if (xSemaphoreTake(xSemaphoreMutex, portMAX_DELAY) == pdTRUE) {
            MPU6050_Signals data;
            xQueueReceive(xMPU6050SignalsDataQueue, &data, portMAX_DELAY);

            JSONVar jsonVar;
            jsonVar["header"] = "MPU6050_SIGNALS_DATA";
            jsonVar["accelXSignals"] = convertToString(data.accelXSignals, 256);
            jsonVar["accelYSignals"] = convertToString(data.accelYSignals, 256);
            jsonVar["accelZSignals"] = convertToString(data.accelZSignals, 256);
            jsonVar["gyroXSignals"] = convertToString(data.gyroXSignals, 256);
            jsonVar["gyroYSignals"] = convertToString(data.gyroYSignals, 256);
            jsonVar["gyroZSignals"] = convertToString(data.gyroZSignals, 256);
            client.send(JSON.stringify(jsonVar));

            releaseMPU6050SignalsMemory(data);
            start = false;
            isMPU6050Completed = false;

            xSemaphoreGive(xSemaphoreMutex);
          }
        }

      } else {
        if (isMPU6050Completed && isMAX30102Completed) {
          if (xSemaphoreTake(xSemaphoreMutex, portMAX_DELAY) == pdTRUE) {
            AllSignalsData data;
            xQueueReceive(xAllSignalsDataQueue, &data, portMAX_DELAY);

            {
              JSONVar jsonVar;
              jsonVar["header"] = "ALL_SIGNALS_DATA";
              jsonVar["mpu6050Signals"]["accelXSignals"] = convertToString(data.mpu6050Signals.accelXSignals, 1024);
              delete[] data.mpu6050Signals.accelXSignals;
              client.send(JSON.stringify(jsonVar));
            }

            {
              JSONVar jsonVar;
              jsonVar["header"] = "ALL_SIGNALS_DATA";
              jsonVar["mpu6050Signals"]["accelYSignals"] = convertToString(data.mpu6050Signals.accelYSignals, 1024);
              delete[] data.mpu6050Signals.accelYSignals;
              client.send(JSON.stringify(jsonVar));
            }

            {
              JSONVar jsonVar;
              jsonVar["header"] = "ALL_SIGNALS_DATA";
              jsonVar["mpu6050Signals"]["accelZSignals"] = convertToString(data.mpu6050Signals.accelZSignals, 1024);
              delete[] data.mpu6050Signals.accelZSignals;
              client.send(JSON.stringify(jsonVar));
            }

            {
              JSONVar jsonVar;
              jsonVar["header"] = "ALL_SIGNALS_DATA";
              jsonVar["mpu6050Signals"]["gyroXSignals"] = convertToString(data.mpu6050Signals.gyroXSignals, 1024);
              delete[] data.mpu6050Signals.gyroXSignals;
              client.send(JSON.stringify(jsonVar));
            }

            {
              JSONVar jsonVar;
              jsonVar["header"] = "ALL_SIGNALS_DATA";
              jsonVar["mpu6050Signals"]["gyroYSignals"] = convertToString(data.mpu6050Signals.gyroYSignals, 1024);
              delete[] data.mpu6050Signals.gyroYSignals;
              client.send(JSON.stringify(jsonVar));
            }

            {
              JSONVar jsonVar;
              jsonVar["header"] = "ALL_SIGNALS_DATA";
              jsonVar["mpu6050Signals"]["gyroZSignals"] = convertToString(data.mpu6050Signals.gyroZSignals, 1024);
              delete[] data.mpu6050Signals.gyroZSignals;
              client.send(JSON.stringify(jsonVar));
            }

            {
              JSONVar jsonVar;
              jsonVar["header"] = "ALL_SIGNALS_DATA";
              jsonVar["ppgSignal"] = convertToString(data.ppgSignal, 1024);
              jsonVar["subject"] = "SleepStage";
              delete[] data.ppgSignal;
              client.send(JSON.stringify(jsonVar));
            }

            start = false;
            isMPU6050Completed = false;
            isMAX30102Completed = false;

            xSemaphoreGive(xSemaphoreMutex);
          }
        }
      }
    }



    delay(10);
  }
}

#include <Arduino_JSON.h>
#include "my_structs.h"
#include <math.h>
#include "human_activity_recognition_model.h"
#include "sleep_stage_recognition_model.h"
#include <arduinoFFT.h>
#include <LiquidCrystal_I2C.h>

extern SemaphoreHandle_t xSemaphoreMutex;
extern QueueHandle_t xAllSignalsDataQueue;
extern boolean isMPU6050Completed;
extern boolean isMAX30102Completed;
extern LiquidCrystal_I2C lcd;

Eloquent::ML::Port::Human_Activity_SVM Human_Activity_Classifier;
Eloquent::ML::Port::Sleep_Stage_SVM Sleep_Stage_Classifier;

void predict_setup() {
  
}

float roundTo(float number, int digits) {
  float factor = pow(10, digits);
  int rounded_number = round(number * factor);
  return rounded_number / factor;
}

float _sum(float* data, int size) {
  float res = 0;
  for (int i = 0; i < size; ++i) {
    res += data[i];
  }
  return res;
}

float _mean(float* data, int size) {
  return _sum(data, size) / size;
}

float _std(float* data, int size) {
  float std = 0.0;
  float mean = _mean(data, size);
  for (int i = 0; i < size; i++) {
    std += pow(data[i] - mean, 2);
  }
  return sqrt(std / (size - 1));
}

float max_value(float* data, int size) {
  float res = data[0];
  for (int i = 1; i < size; i++) {
    res = max(res, data[i]);
  }
  return res;
}

float min_value(float* data, int size) {
  float res = data[0];
  for (int i = 1; i < size; i++) {
    res = min(res, data[i]);
  }
  return res;
}

float* slice(float* data, int size, int begin) {
  float* res = new float[size];
  int index = 0;
  for (int i = begin; i < begin + size; ++i) {
    res[index++] = data[i];
  }
  return res;
}

int* predict_human_activity(MPU6050_Signals data) {
  int* res = new int[4];

  for (int i = 0; i < 4; ++i) {
    float* accelX = slice(data.accelXSignals, 256, i * 256);
    float* accelY = slice(data.accelYSignals, 256, i * 256);
    float* accelZ = slice(data.accelZSignals, 256, i * 256);
    float* gyroX = slice(data.gyroXSignals, 256, i * 256);
    float* gyroY = slice(data.gyroYSignals, 256, i * 256);
    float* gyroZ = slice(data.gyroZSignals, 256, i * 256);

    float features[] = {
      roundTo(_mean(accelX, 256), 8),
      roundTo(_mean(accelY, 256), 8),
      roundTo(_mean(accelZ, 256), 8),
      roundTo(_mean(gyroX, 256), 8),
      roundTo(_mean(gyroY, 256), 8),
      roundTo(_mean(gyroZ, 256), 8),
      roundTo(_std(accelX, 256), 8),
      roundTo(_std(accelY, 256), 8),
      roundTo(_std(accelZ, 256), 8),
      roundTo(_std(gyroX, 256), 8),
      roundTo(_std(gyroY, 256), 8),
      roundTo(_std(gyroZ, 256), 8),
      roundTo(min_value(accelX, 256), 8),
      roundTo(min_value(accelY, 256), 8),
      roundTo(min_value(accelZ, 256), 8),
      roundTo(min_value(gyroX, 256), 8),
      roundTo(min_value(gyroY, 256), 8),
      roundTo(min_value(gyroZ, 256), 8),
      roundTo(max_value(accelX, 256), 8),
      roundTo(max_value(accelY, 256), 8),
      roundTo(max_value(accelZ, 256), 8),
      roundTo(max_value(gyroX, 256), 8),
      roundTo(max_value(gyroY, 256), 8),
      roundTo(max_value(gyroZ, 256), 8)
    };

    res[i] = Human_Activity_Classifier.predict(features);
  }

  return res;
}

float* extract_feature_from_activities(int* activities) {
  float* res = new float[4]{ 0, 0, 0, 0 };
  for (int i = 0; i < 4; ++i) {
    if (activities[i] == 0) {
      res[0] = 1;
    } else if (activities[i] == 1) {
      res[2] = 1;
    } else if (activities[i] == 2) {
      res[1] = 1;
    } else {
      res[3] = 1;
    }
  }
  return res;
}

void ppgSignalNormalize(float* data) {
  float mean = _mean(data, 1024);
  for (int i = 0; i < 1024; ++i) {
    data[i] -= mean;
  }
}

void build_raw_data(float* signal_data, float* vReal, float* vImag, int size) {
  for (int i = 0; i < size; i++) {
    vReal[i] = signal_data[i];
    vImag[i] = 0;
  }
}

String convert_to_activity_label(int* idx) {
  String res;
  for (int i = 0; i < 4; ++i) {
    boolean check = false;
    for (int j = 0; j < i; ++j) {
      if (idx[i] == idx[j]) {
        check = true;
        break;
      }
    }

    if (check) {
      continue;
    }
    
    res += String(Human_Activity_Classifier.idxToLabel(idx[i])).substring(0, 3) + " ";
  }
  return res;
}

float majorPeak(float* data, int fs, int n, int begin, int end) {
  int res = begin;
  float max_val = data[begin];
  for (int i = begin + 1; i < end; i++) {
    if (data[i] > max_val) {
      max_val = data[i];
      res = i;
    }
  }
  return (float) res*1.0/n*fs;
}

void writeLCD(String activities, String stage) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACT: " + activities);
  lcd.setCursor(0, 1);
  lcd.print("STAGE: " + stage);
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

void predict_task(void* pvParameters) {
  (void)pvParameters;
  predict_setup();

  while (true) {
    // Send signals data to ws server
    if (isMPU6050Completed && isMAX30102Completed) {
      if (xSemaphoreTake(xSemaphoreMutex, portMAX_DELAY) == pdTRUE) {
        AllSignalsData data;
        xQueueReceive(xAllSignalsDataQueue, &data, portMAX_DELAY);

        // Human Activity Prediction
        int* activities = predict_human_activity(data.mpu6050Signals);

        // Sleep Stage Prediction
        //// Calc the mpu6050 signals
        float* features = new float[25] {
          roundTo(_mean(data.mpu6050Signals.accelXSignals, 1024), 8),
          roundTo(_mean(data.mpu6050Signals.accelYSignals, 1024), 8),
          roundTo(_mean(data.mpu6050Signals.accelZSignals, 1024), 8),
          roundTo(_mean(data.mpu6050Signals.gyroXSignals, 1024), 8),
          roundTo(_mean(data.mpu6050Signals.gyroYSignals, 1024), 8),
          roundTo(_mean(data.mpu6050Signals.gyroZSignals, 1024), 8),
          roundTo(_std(data.mpu6050Signals.accelXSignals, 1024), 8),
          roundTo(_std(data.mpu6050Signals.accelYSignals, 1024), 8),
          roundTo(_std(data.mpu6050Signals.accelZSignals, 1024), 8),
          roundTo(_std(data.mpu6050Signals.gyroXSignals, 1024), 8),
          roundTo(_std(data.mpu6050Signals.gyroYSignals, 1024), 8),
          roundTo(_std(data.mpu6050Signals.gyroZSignals, 1024), 8),
          roundTo(min_value(data.mpu6050Signals.accelXSignals, 1024), 8),
          roundTo(min_value(data.mpu6050Signals.accelYSignals, 1024), 8),
          roundTo(min_value(data.mpu6050Signals.accelZSignals, 1024), 8),
          roundTo(min_value(data.mpu6050Signals.gyroXSignals, 1024), 8),
          roundTo(min_value(data.mpu6050Signals.gyroYSignals, 1024), 8),
          roundTo(min_value(data.mpu6050Signals.gyroZSignals, 1024), 8),
          roundTo(max_value(data.mpu6050Signals.accelXSignals, 1024), 8),
          roundTo(max_value(data.mpu6050Signals.accelYSignals, 1024), 8),
          roundTo(max_value(data.mpu6050Signals.accelZSignals, 1024), 8),
          roundTo(max_value(data.mpu6050Signals.gyroXSignals, 1024), 8),
          roundTo(max_value(data.mpu6050Signals.gyroYSignals, 1024), 8),
          roundTo(max_value(data.mpu6050Signals.gyroZSignals, 1024), 8)
        };

        //// PPG Signal Normalization
        ppgSignalNormalize(data.ppgSignal);

        //// FFT
        float* vReal = new float[1024];
        float* vImag = new float[1024];
        ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, 1024, 50);
        build_raw_data(data.ppgSignal, vReal, vImag, 1024);
        FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
        FFT.compute(FFTDirection::Forward);
        FFT.complexToMagnitude();

        //// Calc the feature values
        features[24] = roundTo(majorPeak(vReal, 50, 1024, 14, 40)*60, 0);
        // Serial.println(String(features[24])); 

        Serial.println(roundTo(_mean(data.mpu6050Signals.accelXSignals, 1024), 8));
        Serial.println(roundTo(_mean(data.mpu6050Signals.accelYSignals, 1024), 8));
        Serial.println(roundTo(_mean(data.mpu6050Signals.accelZSignals, 1024), 8));
        Serial.println(roundTo(_mean(data.mpu6050Signals.gyroXSignals, 1024), 8));
        Serial.println(roundTo(_mean(data.mpu6050Signals.gyroYSignals, 1024), 8));
        Serial.println(roundTo(_mean(data.mpu6050Signals.gyroZSignals, 1024), 8));
        Serial.println(roundTo(_std(data.mpu6050Signals.accelXSignals, 1024), 8));
        Serial.println(roundTo(_std(data.mpu6050Signals.accelYSignals, 1024), 8));
        Serial.println(roundTo(_std(data.mpu6050Signals.accelZSignals, 1024), 8));
        Serial.println(roundTo(_std(data.mpu6050Signals.gyroXSignals, 1024), 8));
        Serial.println(roundTo(_std(data.mpu6050Signals.gyroYSignals, 1024), 8));
        Serial.println(roundTo(_std(data.mpu6050Signals.gyroZSignals, 1024), 8));
        Serial.println(roundTo(min_value(data.mpu6050Signals.accelXSignals, 1024), 8));
        Serial.println(roundTo(min_value(data.mpu6050Signals.accelYSignals, 1024), 8));
        Serial.println(roundTo(min_value(data.mpu6050Signals.accelZSignals, 1024), 8));
        Serial.println(roundTo(min_value(data.mpu6050Signals.gyroXSignals, 1024), 8));
        Serial.println(roundTo(min_value(data.mpu6050Signals.gyroYSignals, 1024), 8));
        Serial.println(roundTo(min_value(data.mpu6050Signals.gyroZSignals, 1024), 8));
        Serial.println(roundTo(max_value(data.mpu6050Signals.accelXSignals, 1024), 8));
        Serial.println(roundTo(max_value(data.mpu6050Signals.accelYSignals, 1024), 8));
        Serial.println(roundTo(max_value(data.mpu6050Signals.accelZSignals, 1024), 8));
        Serial.println(roundTo(max_value(data.mpu6050Signals.gyroXSignals, 1024), 8));
        Serial.println(roundTo(max_value(data.mpu6050Signals.gyroYSignals, 1024), 8));
        Serial.println(roundTo(max_value(data.mpu6050Signals.gyroZSignals, 1024), 8));
        Serial.println(features[24]);

        writeLCD(convert_to_activity_label(activities), Sleep_Stage_Classifier.predictLabel(features));

        releaseAllSignalsDataMemory(data);
        delete[] activities;
        delete[] features;
        delete[] vReal;
        delete[] vImag;

        isMPU6050Completed = false;
        isMAX30102Completed = false;

        xSemaphoreGive(xSemaphoreMutex);
      }
    }

    delay(10);
  }
}

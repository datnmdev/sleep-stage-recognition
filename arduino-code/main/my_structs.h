#ifndef MY_STRUCTS_H
#define MY_STRUCTS_H

typedef struct {
  float *accelXSignals;
  float *accelYSignals;
  float *accelZSignals;
  float *gyroXSignals;
  float *gyroYSignals;
  float *gyroZSignals;
} MPU6050_Signals;

typedef struct {
  MPU6050_Signals mpu6050Signals;
  float *ppgSignal;
} AllSignalsData;

#endif
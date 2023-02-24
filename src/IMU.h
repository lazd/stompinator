#ifndef __imu_h__
#define __imu_h__

#include <CircularBuffer.h>
#include <M5Core2.h>

#include <Filters/SMA.hpp>
#include "Constants.h"

class IMU {
private:
  bool running;

  // Flat calibration values
  float baseAccZ;

  // last raw acceleration values
  float accX;
  float accY;
  float accZ;

  // Buffer of the last readings
  CircularBuffer<float, IMUBUFFERSIZE> accZBuffer;

  // Moving average to smooth readings
  SMA<SMASIZE, float, float> accZAverage = {1};
  SMA<CALIBRATIONSMASIZE, float, float> baseAccZAverage = {1};

  static void update(void *param) {
    IMU *imu = (IMU *)param;
    imu->running = true;
    while(true) {
      if (imu->running) {
        // Pull IMU data and offset by calibration value
        M5.IMU.getAccelData(&imu->accX, &imu->accY, &imu->accZ);
        imu->accZBuffer.push(abs(1 - imu->accZAverage(abs(imu->accZ - imu->baseAccZ))) * SENSITIVITY);
      }
      vTaskDelay(TICKTIME / portTICK_PERIOD_MS);
    }
  }
public:
  float getAcc() {
    return (1 - abs(accZ - baseAccZ)) * SENSITIVITY;
  }

  int size() {
    return accZBuffer.size();
  }

  float pop() {
    return accZBuffer.pop();
  }

  void clear() {
    accZBuffer.clear();
  }

  void pause() {
    running = false;
  }

  void resume() {
    running = true;
  }

  boolean isEmpty() {
    return accZBuffer.isEmpty();
  }

  void start() {
    // Init IMU and store calibration values
    M5.IMU.Init();
    for (int i = 0; i < CALIBRATIONSMASIZE; i++) {
      delay(TICKTIME);
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      baseAccZ = baseAccZAverage(accZ) - 1;
    }

    TaskHandle_t imuUpdateTaskHandle;
    xTaskCreatePinnedToCore(this->update, "IMU Update Task", 4096, this, 1, &imuUpdateTaskHandle, 1);
  }
};

#endif

#ifndef __imu_h__
#define __imu_h__

#include <CircularBuffer.h>
#include <M5Core2.h>

#include <Filters/SMA.hpp>
#include "Constants.h"

class IMU {
private:
  bool running;

  // Raw acceleration values
  // Buffer of the last readings
  CircularBuffer<float, IMUBUFFERSIZE> intensityBuffer;

  static void update(void *param) {
    IMU *imu = (IMU *)param;
    imu->running = true;

    // Store raw read values
    float accX;
    float accY;
    float accZ;

    // Moving average to smooth readings
    SMA<SMASIZE, float, float> accZAverage = {1};

    // Get calibration
    float baseAccZ;
    SMA<CALIBRATIONSMASIZE, float, float> baseAccZAverage = {1};
    for (int i = 0; i < CALIBRATIONSMASIZE; i++) {
      delay(TICKTIME);
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      baseAccZ = baseAccZAverage(accZ) - 1;
    }

    while(true) {
      if (imu->running) {
        // Pull IMU data and offset by calibration value
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        imu->intensityBuffer.push(abs(1 - accZAverage(abs(accZ - baseAccZ))) * SENSITIVITY);
      }
      vTaskDelay(TICKTIME / portTICK_PERIOD_MS);
    }
  }
public:
  float last() {
    return intensityBuffer.last();
  }

  int size() {
    return intensityBuffer.size();
  }

  float pop() {
    return intensityBuffer.pop();
  }

  void clear() {
    intensityBuffer.clear();
  }

  void pause() {
    running = false;
  }

  void resume() {
    running = true;
  }

  boolean isEmpty() {
    return intensityBuffer.isEmpty();
  }

  void start() {
    // Init IMU and store calibration values
    if (M5.IMU.Init() != 0) {
      M5.Lcd.println("IMU Check failed");
      Serial.println("IMU Check failed");
      return;
    }

    TaskHandle_t imuUpdateTaskHandle;
    xTaskCreatePinnedToCore(this->update, "IMU Update Task", 4096, this, 1, &imuUpdateTaskHandle, 0);
  }
};

#endif

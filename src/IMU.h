#ifndef __imu_h__
#define __imu_h__

#include <CircularBuffer.h>
#include <M5Core2.h>

#include <Filters/SMA.hpp>

#define INTENSITY_FACTOR 6

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
  CircularBuffer<float, 32> accZBuffer;

  // Moving average to smooth readings
  SMA<5, float, float> accZAverage = {1};
  SMA<10, float, float> baseAccZAverage = {1};

  static void update(void *param) {
    IMU *imu = (IMU *)param;
    imu->running = true;
    while(true) {
      if (imu->running) {
        // Pull IMU data and offset by calibration value
        M5.IMU.getAccelData(&imu->accX, &imu->accY, &imu->accZ);
        imu->accZBuffer.push(abs(1 - imu->accZAverage(abs(imu->accZ - imu->baseAccZ))) * INTENSITY_FACTOR);
      }
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
public:
  float getAcc() {
    return (1 - abs(accZ - baseAccZ)) * INTENSITY_FACTOR;
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
    for (int i = 0; i < 10; i++) {
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      baseAccZ = baseAccZAverage(accZ) - 1;
      delay(10);
    }

    TaskHandle_t imuUpdateTaskHandle;
    xTaskCreatePinnedToCore(this->update, "IMU Update Task", 4096, this, 1, &imuUpdateTaskHandle, 1);
  }
};

#endif

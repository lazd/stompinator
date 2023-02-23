#ifndef __imu_h__
#define __imu_h__

#include <CircularBuffer.h>
#include <M5Core2.h>

#include <Filters/SMA.hpp>

#define INTENSITY_FACTOR 6

class IMU {
private:
  // last raw acceleration values
  float accX;
  float accY;
  float accZ;

  // Buffer of the last readings
  CircularBuffer<float, 32> accZBuffer;

  // Moving average to smooth readings
  SMA<5, float, float> accZAverage = {1};
  SMA<10, float, float> baseAccZAverage = {1};

  // Flat calibration values
  float baseAccZ;

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
  }

  void update() {
    // Pull IMU data and offset by calibration value
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    accZBuffer.push(abs(1 - accZAverage(abs(accZ - baseAccZ))) * INTENSITY_FACTOR);
  }
};

#endif

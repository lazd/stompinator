#ifndef __imu_h__
#define __imu_h__

#include <M5Core2.h>

#include <Filters/SMA.hpp>

#include "Constants.h"

class IMU {
private:
  QueueHandle_t dataQueue;

  static void update(void *param) {
    IMU *imu = (IMU *)param;

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

    while (true) {
      // Pull IMU data and offset by calibration value
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      float intensity = abs(1 - accZAverage(abs(accZ - baseAccZ))) * SENSITIVITY;
      xQueueSend(imu->dataQueue, &intensity, 0);
      vTaskDelay(pdMS_TO_TICKS(TICKTIME));
    }
  }

public:
  void start(QueueHandle_t dataQueue) {
    this->dataQueue = dataQueue;

    // Init IMU and store calibration values
    if (M5.IMU.Init() != 0) {
      M5.Lcd.println("IMU Check failed");
      Serial.println("IMU Check failed");
      return;
    }

    TaskHandle_t imuUpdateTaskHandle;
    xTaskCreatePinnedToCore(this->update, "IMU Update Task", 8192, this, 1, &imuUpdateTaskHandle, 1);
  }
};

#endif

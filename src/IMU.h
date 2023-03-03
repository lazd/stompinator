#ifndef __imu_h__
#define __imu_h__

#include <M5Core2.h>

#include <Filters/SMA.hpp>

#include "Constants.h"

class IMU {
private:
  QueueHandle_t dataQueue;
  bool calibrationRequired = true;

  static void handleCalibrationEvent(void *arg, esp_event_base_t base, int32_t id, void *eventData) {
    Serial.println("Calibrating IMU");
    IMU *imu = (IMU *)arg;
    imu->calibrate();
  }

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

    auto calibrate = [&]() {
      SMA<CALIBRATIONSMASIZE, float, float> baseAccZAverage = {1};
      for (int i = 0; i < CALIBRATIONSMASIZE; i++) {
        if (i != 0) {
          vTaskDelay(pdMS_TO_TICKS(TICKTIME));
        }
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        baseAccZ = baseAccZAverage(accZ) - 1;
      }
    };

    while (true) {
      if (imu->calibrationRequired) {
        calibrate();
        imu->calibrationRequired = false;
      }

      // Pull IMU data and offset by calibration value
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      float intensity = abs(1 - accZAverage(abs(accZ - baseAccZ))) * SENSITIVITY;
      xQueueSend(imu->dataQueue, &intensity, 0);
      vTaskDelay(pdMS_TO_TICKS(TICKTIME));
    }
  }

public:
  void calibrate() {
    this->calibrationRequired = true;
  }

  void start(esp_event_loop_handle_t loopHandle, QueueHandle_t dataQueue) {
    esp_event_handler_register_with(loopHandle, IMU_EVENT, IMU_CALIBRATE, handleCalibrationEvent, this);

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

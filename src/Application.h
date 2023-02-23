#ifndef __application_h__
#define __application_h__

#include "IMU.h"
#include "UI.h"
#include "WiFiManager.h"
#include "RTCManager.h"
#include "Logger.h"

class Application {
private:
  WiFiManager *wifi;
  RTCManager *rtc;
  IMU *imu;
  UI *ui;
  Logger *logger;

public:
  Application() {
    this->wifi = new WiFiManager();
    this->rtc = new RTCManager();
    this->imu = new IMU();
    this->ui = new UI();
    this->logger = new Logger();
  }

  void start() {
    wifi->start();
    rtc->start();
    imu->start();
    ui->start(this->imu);
    logger->start(this->imu, this->rtc);
  }

  void loop() {
    imu->update();
    rtc->update();
    ui->update();
    logger->update();
  }
};

#endif

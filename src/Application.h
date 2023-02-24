#ifndef __application_h__
#define __application_h__

#include "IMU.h"
#include "UI.h"
// #include "WebServer.h"
#include "WiFiManager.h"
#include "RTCManager.h"
#include "Logger.h"

class Application {
private:
  // WebServer *webServer;
  WiFiManager *wifi;
  RTCManager *rtc;
  IMU *imu;
  UI *ui;
  Logger *logger;

public:
  Application() {
    wifi = new WiFiManager();
    rtc = new RTCManager();
    imu = new IMU();
    ui = new UI();
    logger = new Logger();
    // webServer = new WebServer();
  }

  void start() {
    wifi->start();
    rtc->start();
    imu->start();
    ui->start();
    logger->start(rtc);
    // webServer->start();
  }

  void loop() {
    rtc->update();

    // Get data from IMU and pass to consumers
    imu->pause();
    int size = imu->size();
    float data[size];
    for (int i = size - 1; i >= 0; i--) {
      data[i] = imu->pop();
    }
    imu->clear();
    imu->resume();

    logger->update(data, size);
    ui->update(data, size);
    // webServer->update(data, size);
  }
};

#endif

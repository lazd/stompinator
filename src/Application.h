#ifndef __application_h__
#define __application_h__

#include "IMU.h"
#include "UI.h"
#include "WebServer.h"
#include "WiFiManager.h"
#include "RTCManager.h"
#include "Logger.h"

class Application {
private:
  WebServer *webServer;
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
    webServer = new WebServer();
  }

  void start() {
    wifi->start();
    rtc->start();
    imu->start();
    ui->start();
    logger->start(imu, rtc);
    webServer->start();
  }

  void loop() {
    // imu->update();
    rtc->update();
    logger->update();

    // Get data from IMU and pass to UI and webserver
    int size = imu->size();
    float data[size];
    for (int i = 0; i < size; i++) {
      data[i] = imu->pop();
    }
    imu->clear();

    ui->update(data, size);
    webServer->update(data, size);
  }
};

#endif

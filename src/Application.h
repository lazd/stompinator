#ifndef __application_h__
#define __application_h__

#include "WiFiManager.h"
#include "RTCManager.h"
#include "IMU.h"
#include "UI.h"
#include "Watcher.h"
// #include "WebServer.h"
// #include "Logger.h"

class Application {
private:
  // WebServer *webServer;
  Watcher *watcher;
  WiFiManager *wifi;
  RTCManager *rtc;
  IMU *imu;
  UI *ui;
  // Logger *logger;

public:
  Application() {
    wifi = new WiFiManager();
    rtc = new RTCManager();
    imu = new IMU();
    ui = new UI();
    watcher = new Watcher();
    // logger = new Logger();
    // webServer = new WebServer();
  }

  void start() {
    wifi->start();
    rtc->start();
    imu->start();
    ui->start();
    watcher->start(rtc);
    // logger->start(rtc);
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

    ui->update(data, size);
    watcher->update(data, size);
    // logger->update(data, size);
    // webServer->update(data, size);
  }
};

#endif

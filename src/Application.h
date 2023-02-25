#ifndef __application_h__
#define __application_h__

#include "IMU.h"
#include "RTCManager.h"
#include "UI.h"
#include "Watcher.h"
#include "WiFiManager.h"
// #include "Logger.h"
// #include "WebServer.h"

class Application {
private:
  // Logger *logger;
  // WebServer *webServer;
  Watcher *watcher;
  WiFiManager *wifi;
  RTCManager *rtc;
  IMU *imu;
  UI *ui;

  // Store data to be passed to consumers
  unsigned int dataSize;
  float data[IMUBUFFERSIZE];

  // Store data to be recieved from IMU
  QueueHandle_t dataQueue;

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
    dataQueue = xQueueCreate(IMUBUFFERSIZE, sizeof(float));

    wifi->start();
    rtc->start();
    imu->start(dataQueue);
    ui->start();
    watcher->start(rtc);
    // logger->start(rtc);
    // webServer->start();
  }

  void loop() {
    rtc->update();

    // Get data from IMU and pass to consumers
    float intensity;
    this->dataSize = 0;
    while (xQueueReceive(dataQueue, &intensity, 0)) {
      this->data[this->dataSize++] = intensity;
    }

    ui->update(this->data, this->dataSize);
    watcher->update(this->data, this->dataSize);
    // logger->update(this->data, this->dataSize);
    // webServer->update(this->data, this->dataSize);
  }
};

#endif

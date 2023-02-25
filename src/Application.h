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
    this->wifi = new WiFiManager();
    this->rtc = new RTCManager();
    this->imu = new IMU();
    this->ui = new UI();
    this->watcher = new Watcher();
    // this->logger = new Logger();
    // this->webServer = new WebServer();
  }

  void start() {
    this->dataQueue = xQueueCreate(IMUBUFFERSIZE, sizeof(float));

    this->wifi->start();
    this->rtc->start();
    this->imu->start(this->dataQueue);
    this->ui->start();
    this->watcher->start(rtc);
    // this->logger->start(this->rtc);
    // this->webServer->start();
  }

  void loop() {
    this->rtc->update();

    // Get data from IMU and pass to consumers
    float intensity;
    this->dataSize = 0;
    while (xQueueReceive(dataQueue, &intensity, 0)) {
      this->data[this->dataSize++] = intensity;
    }

    this->ui->update(this->data, this->dataSize);
    this->watcher->update(this->data, this->dataSize);
    // logger->update(this->data, this->dataSize);
    // webServer->update(this->data, this->dataSize);
  }
};

#endif

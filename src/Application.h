#ifndef __application_h__
#define __application_h__

#include "IMU.h"
#include "NTP.h"
#include "UI.h"
#include "Watcher.h"
#include "WiFiManager.h"
// #include "WebServer.h"

class Application {
private:
  // WebServer *webServer;
  NTP *ntp;
  Watcher *watcher;
  WiFiManager *wifi;
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
    this->ntp = new NTP();
    this->imu = new IMU();
    this->ui = new UI();
    this->watcher = new Watcher();
    // this->webServer = new WebServer();
  }

  void start() {
    this->dataQueue = xQueueCreate(IMUBUFFERSIZE, sizeof(float));

    this->wifi->start();
    this->ntp->start();
    this->imu->start(this->dataQueue);
    this->ui->start();
    this->watcher->start();
    // this->webServer->start();
  }

  void loop() {

    // Get data from IMU and pass to consumers
    float intensity;
    this->dataSize = 0;
    while (xQueueReceive(dataQueue, &intensity, 0)) {
      this->data[this->dataSize++] = intensity;
    }

    this->ui->update(this->data, this->dataSize);
    this->watcher->update(this->data, this->dataSize);
    // webServer->update(this->data, this->dataSize);
  }
};

#endif

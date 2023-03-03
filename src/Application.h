#ifndef __application_h__
#define __application_h__

#include "Constants.h"
#include "IMU.h"
#include "NTP.h"
#include "UI.h"
#include "Watcher.h"
#include "WiFiManager.h"

#ifdef WEBSERVER
#include "WebServer.h"
#endif

void handleCalibrationEvent(void *arg, esp_event_base_t base, int32_t id, void *eventData) {
  Serial.println("Calibrating IMU");
  IMU *imu = (IMU *)arg;
  imu->calibrate();
}

class Application {
private:
#ifdef WEBSERVER
  WebServer *webServer;
#endif

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
#ifdef WEBSERVER
    this->webServer = new WebServer();
#endif
  }

  void start() {
    this->dataQueue = xQueueCreate(IMUBUFFERSIZE, sizeof(float));

    esp_event_loop_args_t loopArgs = {
        .queue_size = 5,
        .task_name = "eventLoop",
        .task_priority = 3,
        .task_stack_size = 1024,
        .task_core_id = 1};

    esp_event_loop_handle_t loopHandle;
    esp_event_loop_create(&loopArgs, &loopHandle);
    esp_event_handler_register_with(loopHandle, IMU_EVENT, IMU_CALIBRATE, handleCalibrationEvent, this->imu);

    this->wifi->start();
    this->ntp->start();
    this->imu->start(this->dataQueue);
    this->ui->start();
    this->watcher->start();
#ifdef WEBSERVER
    this->webServer->start(loopHandle);
#endif
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
#ifdef WEBSERVER
    webServer->update(this->data, this->dataSize);
#endif
  }
};

#endif

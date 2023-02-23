#ifndef __logger_h__
#define __logger_h__

#include <Arduino.h>

#include "IMU.h"
#include "RTCManager.h"

class Logger {
private:
  IMU *imu;
  RTCManager *rtc;
  File logFile;
  bool loggingEnabled;
  uint logYear;
  uint logMonth;
  uint logDate;

  void openLogFile() {
    if (!this->loggingEnabled) {
      return;
    }

    if (!this->logFile || !rtc->dateIs(this->logYear, this->logMonth, this->logDate)) {
      if (this->logFile) {
        this->logFile.close();
      }

      char dateString[DATESTRINGLENGTH];
      rtc->getDateString(dateString);
      char fileName[DATETIMESTRINGLENGTH];
      sprintf(fileName, "/log-%s.txt", dateString);

      this->logFile = SD.open(fileName, FILE_APPEND);
      if (this->logFile) {
        M5.Lcd.printf("Log %s\n", fileName);
        this->loggingEnabled = true;
      }
      else {
        M5.Lcd.println("Failed to open log file");
        this->loggingEnabled = false;
      }

      this->logYear = rtc->getYear();
      this->logMonth = rtc->getMonth();
      this->logDate = rtc->getDate();
    }
  }

public:
  void start(IMU* imu, RTCManager *rtc) {
    this->imu = imu;
    this->rtc = rtc;

    this->loggingEnabled = false;

    if (SD.begin()) {
      M5.Lcd.println("SD card mounted");
      this->loggingEnabled = true;
      this->openLogFile();
    } else {
      M5.Lcd.println("Failed to mount SD card");
    }
  }

  void update() {
    if (this->loggingEnabled && this->logFile) {
      char timeString[TIMESTRINGLENGTH];
      this->rtc->getTimeString(timeString);

      this->logFile.printf("%s,%5.10f\n", timeString, this->imu->getAcc());
      this->logFile.flush(); // todo: don't flush every time
    }
  }
};

#endif

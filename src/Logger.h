#ifndef __logger_h__
#define __logger_h__

#include <Arduino.h>

#include "RTCManager.h"

class Logger {
private:
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

      char fileName[DATESTRINGLENGTH + 1 + 9];
      sprintf(fileName, "/log-%s.csv", rtc->getDateString());

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
  void start(RTCManager *rtc) {
    this->rtc = rtc;

    this->loggingEnabled = false;

    if (SD.begin()) {
      M5.Lcd.println("SD card mounted");
      this->loggingEnabled = true;
      this->openLogFile();
    }
  }

  void update(float* data, int size) {
    if (this->loggingEnabled && this->logFile) {
      for (int i = 0; i < size; i++) {
        this->logFile.printf("%s,%5.10f\n", this->rtc->getTimeString(), data[i]);
      }
      this->logFile.flush(); // todo: don't flush every time
    }
  }
};

#endif

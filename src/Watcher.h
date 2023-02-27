#ifndef __watcher_h__
#define __watcher_h__

#include <Arduino.h>
#include <CircularBuffer.h>
#include <TimeLib.h>

#include <Filters/MedianFilter.hpp>

#include "Constants.h"

#define FLUSHTIME 5000
#define DATESTRINGLENGTH 10
#define DATETIMESTRINGLENGTH 19
#define TIMESTRINGLENGTH 8
#define INSTANCEFILTERSIZE 50

class Watcher {
private:
  File logFile;
  uint logYear;
  uint logMonth;
  uint logDay;

  bool didInitialSleep = false;
  bool displaySleeping = false;
  bool loggingEnabled = false;

  bool hasActiveInstance = false;
  float instanceMaxIntensity = 0;
  unsigned long lastFlushTime;
  time_t instanceStartTimeSeconds;
  unsigned long instanceStartTime;
  unsigned long instanceEndTime;

  SMA<INSTANCEFILTERSIZE, float, float> instanceAverage = {0};

  void openLogFile() {
    if (!this->loggingEnabled) {
      return;
    }

    if (!this->logFile || this->logYear != year() || this->logMonth != month() || this->logDay != day()) {
      if (this->logFile) {
        this->logFile.close();
      }

      char fileName[DATESTRINGLENGTH + TIMESTRINGLENGTH + 1 + 8];
      sprintf(fileName, "/log-%02d-%02d-%02d.csv", year(), month(), day());

      bool logFileExists = SD.exists(fileName);
      this->logFile = SD.open(fileName, FILE_APPEND);
      if (this->logFile) {
        this->loggingEnabled = true;
        if (!logFileExists) {
          Serial.println("Starting logfile from scratch");
          this->logFile.println("time,intensity,duration");
        }
      } else {
        M5.Lcd.println("Failed to open log file");
        Serial.printf("Failed to open log file %s\n", fileName);
        this->loggingEnabled = false;
      }

      this->logYear = year();
      this->logMonth = month();
      this->logDay = day();
    }
  }

  void logInstance() {
    this->openLogFile();
    Serial.printf("[%02d:%02d:%02d] %5.10f for %dms\n", hour(this->instanceStartTimeSeconds), minute(this->instanceStartTimeSeconds), second(this->instanceStartTimeSeconds), this->instanceMaxIntensity, this->instanceEndTime - this->instanceStartTime);
    if (this->logFile) {
      this->logFile.printf("%02d:%02d:%02d,%5.10f,%d\n", hour(this->instanceStartTimeSeconds), minute(this->instanceStartTimeSeconds), second(this->instanceStartTimeSeconds), this->instanceMaxIntensity, this->instanceEndTime - this->instanceStartTime);
    }
  }

  void flush() {
    if (this->logFile) {
      this->logFile.flush();
      this->lastFlushTime = millis();
    }
  }

  bool isSleeping() {
    return this->displaySleeping;
  }

  void sleep() {
    M5.Lcd.sleep();
    M5.Axp.SetLcdVoltage(SCREENOFFBRIGHTNESS);
    this->displaySleeping = true;
  }

  void wake() {
    M5.Lcd.wakeup();
    M5.Axp.SetLcdVoltage(SCREENONBRIGHTNESS);
    this->displaySleeping = false;
  }

  void startInstance(float intensity) {
    this->wake();
    instanceAverage = {intensity};
    this->instanceMaxIntensity = intensity;
    this->hasActiveInstance = true;
    this->instanceStartTime = millis();
    this->instanceStartTimeSeconds = now();
  }

  void stopInstance() {
    this->hasActiveInstance = false;
    this->instanceEndTime = millis();
    this->logInstance();
  }

  void trackInstance(float intensity) {
    if (this->hasActiveInstance) {
      float currentInstanceAverage = instanceAverage(intensity);
      if (currentInstanceAverage > INSTANCETHRESHOLD) {
        if (intensity > this->instanceMaxIntensity) {
          this->instanceMaxIntensity = intensity;
        }
      } else {
        this->stopInstance();
      }
    } else {
      if (intensity > WATCHTHRESHOLD) {
        this->startInstance(intensity);
      }
    }
  }

public:
  void start() {
    // Set initial brightness
    this->wake();

    if (SD.begin()) {
      this->loggingEnabled = true;

      File sessionFile = SD.open("/sessions.csv", FILE_APPEND);
      if (sessionFile) {
        sessionFile.printf("%02d-%02d-%02d %02d:%02d:%02d\n", year(), month(), day(), hour(), minute(), second());
        sessionFile.close();
      } else {
        Serial.println("Failed to open session file");
        this->loggingEnabled = false;
      }

      this->openLogFile();
    }

    if (!this->loggingEnabled) {
      M5.Lcd.println("Watcher logging disabled");
      Serial.println("Watcher logging disabled");
    }
  }

  void update(float *data, int size) {
    for (int i = 0; i < size; i++) {
      this->trackInstance(data[i]);
    }

    if (millis() - this->lastFlushTime >= FLUSHTIME) {
      this->flush();
    }

    if (!this->hasActiveInstance) {
      if (!this->didInitialSleep && millis() > INITIALWAKETIME) {
        this->sleep();
        this->didInitialSleep = true;
      } else if (this->didInitialSleep && millis() - this->instanceEndTime > EVENTTIMEOUT) {
        this->sleep();
      }
    }
  }
};

#endif

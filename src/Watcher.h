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
  bool didInitialSleep;
  bool displaySleeping;
  bool loggingEnabled;
  bool activeEvent;
  float eventMaxIntensity = 0;
  float currentPartIntensity = 0;
  unsigned long eventStartTime;
  unsigned long lastFlushTime;
  unsigned long lastSignificantInstance;

  bool instanceActive = false;
  float instanceMaxIntensity = 0;
  unsigned long instanceStartTime;

  CircularBuffer<float, REWINDSTEPS> buffer;
  SMA<INSTANCEFILTERSIZE, float, float> instanceAverage = {0};
  // MedianFilter<INSTANCEFILTERSIZE, float> instanceAverage = {0};

  void openLogFile() {
    if (this->loggingEnabled) {
      char fileName[DATESTRINGLENGTH + TIMESTRINGLENGTH + 1 + 12];
      sprintf(fileName, "/event-%02d-%02d-%02d-%02d_%02d_%02d.csv", year(), month(), day(), hour(), minute(), second());

      this->logFile = SD.open(fileName, FILE_WRITE);
      if (!this->logFile) {
        Serial.println("Failed to open event file");
        this->loggingEnabled = false;
      }
    }
  }

  void closeLogFile() {
    if (this->logFile) {
      this->logFile.close();
    }
  }

  void logData(float data) {
    if (this->logFile) {
      this->logFile.printf("%02d:%02d:%02d,%5.10f\n", hour(), minute(), second(), data);
    }
  }

  void logStats() {
    if (this->logFile) {
      this->logFile.printf("max,%5.10f\n", this->eventMaxIntensity);
      this->logFile.printf("duration,%d\n", (millis() - this->eventStartTime) / 1000);
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

public:
  void start() {
    this->activeEvent = false;
    this->loggingEnabled = false;
    this->displaySleeping = false;
    this->didInitialSleep = false;

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
    } else {
      M5.Lcd.println("Watcher logging disabled");
    }
  }

  void startInstance(float intensity) {
    instanceAverage = {intensity};
    this->instanceActive = true;
    this->instanceStartTime = millis();
  }

  void trackInstance(float intensity) {
    if (this->instanceActive) {
      float currentInstanceAverage = instanceAverage(intensity);
      if (currentInstanceAverage > INSTANCETHRESHOLD) {
        if (intensity > this->instanceMaxIntensity) {
          this->instanceMaxIntensity = intensity;
        }
      }
      else {
        this->stopInstance();
      }
    }
    else {
      if (intensity > WATCHTHRESHOLD) {
        this->startInstance(intensity);
      }
    }
  }

  void stopInstance() {
    // Log instance
    this->instanceActive = false;
    Serial.println("Stomp!");
  }

  void update(float *data, int size) {
    for (int i = 0; i < size; i++) {
      if (data[i] > WATCHTHRESHOLD) {
        if (!this->activeEvent) {
          this->wake();

          Serial.printf("Event detected with intensity %f\n", data[i]);
          this->eventStartTime = this->lastSignificantInstance = this->lastFlushTime = millis();
          this->eventMaxIntensity = data[i];

          this->activeEvent = true;
          this->openLogFile();

          this->startInstance(data[i]);

          // Pop the last REWINDSTEPS samples and include them in the log
          float rewindBuffer[REWINDSTEPS];
          int rewindBufferStartIndex = REWINDSTEPS;  // start here in case there is no data
          for (int j = REWINDSTEPS - 1; j >= 0 && !buffer.isEmpty(); j--) {
            rewindBuffer[j] = buffer.pop();
            rewindBufferStartIndex = j;
          }

          // Note: the first REWINDSTEPS samples will have an incorrect timestamp, whatever
          for (int j = rewindBufferStartIndex; j < REWINDSTEPS; j++) {
            this->logData(rewindBuffer[j]);
          }

          buffer.clear();
        } else {
          this->lastSignificantInstance = millis();
        }
      }

      if (this->activeEvent) {
        this->trackInstance(data[i]);

        this->logData(data[i]);
        if (data[i] > this->eventMaxIntensity) {
          this->eventMaxIntensity = data[i];
        }
        if (millis() - this->lastFlushTime >= FLUSHTIME) {
          this->flush();
        }
      } else {
        buffer.push(data[i]);
      }
    }

    if (!this->didInitialSleep && !this->activeEvent && !this->displaySleeping && millis() > INITIALWAKETIME) {
      this->sleep();
      this->didInitialSleep = true;
    }

    if (this->activeEvent && millis() - this->lastSignificantInstance > EVENTTIMEOUT) {
      Serial.printf("Event lasted %d seconds\n", (millis() - this->eventStartTime) / 1000);
      Serial.printf("Max intensity was %f\n", this->eventMaxIntensity);
      this->logStats();
      this->closeLogFile();
      this->sleep();
      this->activeEvent = false;
    }
  }
};

#endif

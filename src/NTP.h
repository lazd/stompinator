#ifndef __ntp_h__
#define __ntp_h__

#include <M5Core2.h>
#include <TimeLib.h>

#include "Constants.h"
#include "time.h"

#define NTPSERVER "pool.ntp.org"

class NTP {
private:
  bool getTimeFromServer() {
    configTime(UTCOFFSETINSECONDS, DAYLIGHTOFFSETINSECONDS, NTPSERVER);
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
      setTime(timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900);
      return true;
    }
    return false;
  }

public:
  void start() {
    M5.Lcd.printf("Syncing to %s\n", NTPSERVER);

    bool timeFetchedSuccessfully = false;
    int timeFetchTries = 0;
    do {
      M5.Lcd.print(".");
      timeFetchedSuccessfully = getTimeFromServer();
      timeFetchTries++;
      delay(500);
    } while (!timeFetchedSuccessfully && timeFetchTries < 5);
    M5.Lcd.print("\n");

    M5.Lcd.printf("%02d-%02d-%02d %02d:%02d:%02d\n", year(), month(), day(), hour(), minute(), second());
    Serial.printf("%02d-%02d-%02d %02d:%02d:%02d\n", year(), month(), day(), hour(), minute(), second());
  }

  void update() {
  }
};

#endif

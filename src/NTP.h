#ifndef __ntp_h__
#define __ntp_h__

#include <M5Core2.h>
#include <TimeLib.h>

#include "Constants.h"
#include "time.h"

#define NTPSERVER "pool.ntp.org"
#define TIMEFETCHTRIES 10

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

  void tryTimeFetch() {
    M5.Lcd.printf("Syncing to %s\n", NTPSERVER);
    Serial.printf("Syncing to %s\n", NTPSERVER);

    bool timeFetchedSuccessfully = false;
    uint fetchTries = 0;
    do {
      M5.Lcd.print(".");
      Serial.print(".");
      timeFetchedSuccessfully = getTimeFromServer();
      delay(500);
      fetchTries++;
    } while (!timeFetchedSuccessfully && fetchTries < TIMEFETCHTRIES);
    M5.Lcd.print("\n");
    Serial.print("\n");

    if (year() == 1970) {
      M5.Lcd.println("Failed to fetch time from server");
      Serial.println("Failed to fetch time from server");

      sleep(1000);
      ESP.restart();
    }
    else {
      M5.Lcd.printf("%02d-%02d-%02d %02d:%02d:%02d\n", year(), month(), day(), hour(), minute(), second());
      Serial.printf("%02d-%02d-%02d %02d:%02d:%02d\n", year(), month(), day(), hour(), minute(), second());
    }
  }

public:
  void start() {
    tryTimeFetch();
  }
};

#endif

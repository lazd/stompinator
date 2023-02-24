#ifndef __rtc_h__
#define __rtc_h__

#include <M5Core2.h>

#include "Constants.h"
#include "time.h"

#define NTPSERVER "pool.ntp.org"
#define DATESTRINGLENGTH 10
#define DATETIMESTRINGLENGTH 19
#define TIMESTRINGLENGTH 8

class RTCManager {
private:
  RTC_DateTypeDef dateStruct;
  RTC_TimeTypeDef timeStruct;

  char dateString[DATESTRINGLENGTH + 1];
  char timeString[TIMESTRINGLENGTH + 1];
  char dateTimeString[DATETIMESTRINGLENGTH + 1];
  char underscoreTimeString[TIMESTRINGLENGTH + 1];

  bool getTimeFromServer() {
    configTime(UTCOFFSETINSECONDS, DAYLIGHTOFFSETINSECONDS, NTPSERVER);
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
      timeStruct.Hours = timeInfo.tm_hour;
      timeStruct.Minutes = timeInfo.tm_min;
      timeStruct.Seconds = timeInfo.tm_sec;
      dateStruct.WeekDay = timeInfo.tm_wday;
      dateStruct.Month = timeInfo.tm_mon + 1;
      dateStruct.Date = timeInfo.tm_mday;
      dateStruct.Year = timeInfo.tm_year + 1900;
      M5.Rtc.SetDate(&dateStruct);
      M5.Rtc.SetTime(&timeStruct);

      return true;
    }
    return false;
  }

public:
  int getYear() {
    return dateStruct.Year;
  }

  int getMonth() {
    return dateStruct.Month;
  }

  int getDate() {
    return dateStruct.Date;
  }

  bool dateIs(int year, int month, int date) {
    return dateStruct.Year == year && dateStruct.Month == month && dateStruct.Date == date;
  }

  char* getDateString() {
    return dateString;
  }

  char* getTimeString() {
    return timeString;
  }

  char* getUnderscoreTimeString() {
    return underscoreTimeString;
  }

  char* getDateTimeString() {
    return dateTimeString;
  }

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
  }

  void update() {
    M5.Rtc.GetDate(&dateStruct);
    M5.Rtc.GetTime(&timeStruct);

    sprintf(this->dateString, "%02d-%02d-%02d", dateStruct.Year, dateStruct.Month, dateStruct.Date);
    sprintf(this->timeString, "%02d:%02d:%02d", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);
    sprintf(this->underscoreTimeString, "%02d_%02d_%02d", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);
    sprintf(this->dateTimeString, "%s %s", this->dateString, this->timeString);
  }
};

#endif

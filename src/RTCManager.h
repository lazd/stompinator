#ifndef __rtc_h__
#define __rtc_h__

#include <M5Core2.h>
#include "time.h"

#define NTPSERVER "pool.ntp.org"
#define UTCOFFSETINSECONDS 8 * -3600
#define DAYLIGHTOFFSETINSECONDS 3600
#define DATESTRINGLENGTH 11
#define DATETIMESTRINGLENGTH 20
#define TIMESTRINGLENGTH 9

class RTCManager {
private:
  RTC_DateTypeDef dateStruct;
  RTC_TimeTypeDef timeStruct;

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

  void getDateString(char* dateString) {
    sprintf(dateString, "%02d-%02d-%02d", dateStruct.Year, dateStruct.Month, dateStruct.Date);
  }

  void getTimeString(char* dateString) {
    sprintf(dateString, "%02d:%02d:%02d", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);
  }

  void getDateTimeString(char* dateTimeString) {
    sprintf(dateTimeString, "%02d-%02d-%02d %02d:%02d:%02d", dateStruct.Year, dateStruct.Month, dateStruct.Date, timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);
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
  }
};

#endif

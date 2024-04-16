#include "esp_event.h"

#ifndef __constants_h__
#define __constants_h__

/* Screen */
// Voltage range: 2500 - 3300
#define SCREENOFFBRIGHTNESS 2500
#define SCREENONBRIGHTNESS 2900

/* IMU */
#define SENSITIVITY 6
#define TICKTIME 6
#define IMUBUFFERSIZE 32
#define SMASIZE 5
#define CALIBRATIONSMASIZE 10

/* RTC */
#define UTCOFFSETINSECONDS 8 * -3600
#define DAYLIGHTOFFSETINSECONDS 3600

/* UI */
#define COLORMULTIPLIER 3
#define TEXTCOLOR GREEN
#define TEXTBACKGROUND BLACK
#define TEXTSIZE 2

/* Watcher */
#define WATCHTHRESHOLD 0.10
#define INSTANCETHRESHOLD 0.06
#define INITIALWAKETIME 20 * 1000  // 20 seconds
#define EVENTTIMEOUT 3 * 1000  // 3 seconds
#define REWINDSTEPS 240 // ~2 seconds
#define MAXFILES 14

struct InstanceInfo {
  time_t startTime;
  unsigned long duration;
  float intensity;
};

/* Config */
#define WEBSERVER

/* Events */
ESP_EVENT_DECLARE_BASE(IMU_EVENT);
ESP_EVENT_DEFINE_BASE(IMU_EVENT);
ESP_EVENT_DECLARE_BASE(WATCHER_EVENT);
ESP_EVENT_DEFINE_BASE(WATCHER_EVENT);

#define IMU_CALIBRATE 1
#define WATCHER_INSTANCE 2

#endif

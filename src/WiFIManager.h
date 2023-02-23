#ifndef __wifi_h__
#define __wifi_h__

#include <M5Core2.h>
#include <WiFi.h>
#include "WiFiSecret.h"

class WiFiManager {
public:
  void start() {
    M5.Lcd.printf("Connecting to %s\n", SSID);
    WiFi.begin(SSID, PASSPHRASE);
    while (WiFi.status() != WL_CONNECTED) {
      M5.Lcd.print(".");
      delay(500);
    }
    M5.Lcd.println("\nConnected!");
  }
};

#endif

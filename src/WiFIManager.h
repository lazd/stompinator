#ifndef __wifi_h__
#define __wifi_h__

#include <M5Core2.h>
#include <WiFi.h>
#include "WiFiSecret.h"

#define WIFICHECKTIME 60 * 1000

class WiFiManager {
public:
  unsigned long lastCheckTime = millis();

  void connect() {
    M5.Lcd.printf("Connecting to %s\n", SSID);
    Serial.printf("Connecting to %s\n", SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSPHRASE);
  }

  void reconnect() {
    Serial.println("Reconnecting...");
    M5.Lcd.println("Reconnecting...");
    this->connect();
  }

  void start() {
    this->connect();

    while (!WiFi.isConnected()) {
      M5.Lcd.print(".");
      delay(500);
    }

    M5.Lcd.printf("\nIP %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("IP %s\n", WiFi.localIP().toString().c_str());
  }

  void update() {
    if (millis() - this->lastCheckTime > WIFICHECKTIME) {
      this->lastCheckTime = millis();
      if (!WiFi.isConnected()) {
        // Just reboot, reconnecting has been flaky
        Serial.println("WiFi disconnected, rebooting");
        M5.Lcd.println("WiFi disconnected, rebooting");

        WiFi.reconnect();
      }
    }
  }
};

#endif

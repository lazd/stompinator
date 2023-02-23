#include "Application.h"

Application *application;

void setup() {
  Serial.begin(115200);

  M5.begin();
  M5.Lcd.setTextColor(TEXTCOLOR, TEXTBACKGROUND);
  M5.Lcd.setTextSize(TEXTSIZE);

  application = new Application();
  application->start();
}

void loop() {
  application->loop();
}

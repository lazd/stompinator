#ifndef __ui_h__
#define __ui_h__

#include <Arduino.h>
#include <M5Core2.h>
#include "IMU.h"

#define WIDTH 320
#define HEIGHT 240
#define TEXTSIZE 2
#define TEXTCOLOR GREEN
#define TEXTBACKGROUND BLACK
#define CENTER HEIGHT / 2
#define TEXT_WIDTH 85
#define TEXT_HEIGHT 21
#define DRAW_INTERVAL 41.66666
#define COLOR_FACTOR 3

uint32_t interpolateColor(float n) {
  float nx = constrain(abs(n) * COLOR_FACTOR, 0, 1);
  float b = 0;
  float g = 0;
  float r = 0;
  if (nx <= 0.5) {  // first, green stays at 100%, red raises to 100%
    g = 1.0;
    r = 2 * nx;
  } else {  // red stays at 100%, green decays
    r = 1.0;
    g = 1.0 - 2 * (nx - 0.5);
  }
  r *= 255;
  g *= 255;
  b *= 255;
  return M5.Lcd.color565(r, g, b);
}

class UI {
private:
  TFT_eSprite *screenBuffer;
  IMU *imu;

  void drawLine(int x, float intensity, uint32_t color) {
    int lineStart = CENTER - intensity * CENTER;
    int lineEnd = CENTER + intensity * CENTER;
    // todo: use fast v line
    this->screenBuffer->drawLine(x, lineStart, x, lineEnd, color);
  }

  void drawGraph() {
    int drawLocation = WIDTH - 1;
    this->screenBuffer->scroll(imu->size() * -1);
    while(!imu->isEmpty()) {
      float dataPoint = imu->pop();
      drawLine(drawLocation, dataPoint, interpolateColor(dataPoint));
      drawLocation--;
    }
  }

  void drawText(float accZ) {
    this->screenBuffer->fillRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT, TEXTBACKGROUND);
    this->screenBuffer->setCursor(0, 0);
    this->screenBuffer->printf("%s%5.3f G", accZ >= 0 ? " " : "", accZ);
  }

public:
  void start(IMU *imu) {
    this->imu = imu;

    // Buffer all screen update operations
    this->screenBuffer = new TFT_eSprite(&M5.Lcd);
    this->screenBuffer->createSprite(WIDTH, HEIGHT);
    this->screenBuffer->setTextColor(TEXTCOLOR, TEXTBACKGROUND);
    this->screenBuffer->setTextSize(TEXTSIZE);
  }

  void update() {
    drawGraph();
    // drawText(imu->getAcc()); // todo: figure out why this crashes?
    this->screenBuffer->pushSprite(0, 0);
  }
};

#endif

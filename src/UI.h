#ifndef __ui_h__
#define __ui_h__

#include <Arduino.h>
#include <M5Core2.h>

#include "Constants.h"

#define WIDTH 320
#define HEIGHT 240
#define CENTER HEIGHT / 2
#define TEXT_WIDTH 85
#define TEXT_HEIGHT 21

uint32_t interpolateColor(float n) {
  float nx = constrain(abs(n) * COLORMULTIPLIER, 0, 1);
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
  TFT_eSprite* screenBuffer;

  void drawLine(int x, float intensity, uint32_t color) {
    int height = min(intensity, 1.0f) * CENTER;
    int lineStart = CENTER - height;
    this->screenBuffer->drawFastVLine(x, lineStart, height * 2, color);
  }

  void drawGraph(float* data, int size) {
    int drawLocation = WIDTH - size;
    this->screenBuffer->scroll(size * -1);
    for (int i = 0; i < size; i++) {
      float dataPoint = data[i];
      drawLine(drawLocation, dataPoint, interpolateColor(dataPoint));
      drawLocation++;
    }
  }

  void drawText(float accZ) {
    this->screenBuffer->fillRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT, TEXTBACKGROUND);
    this->screenBuffer->setCursor(0, 0);
    this->screenBuffer->printf("%s%5.3f G", accZ >= 0 ? " " : "", accZ);
  }

public:
  void start() {
    // Buffer all screen update operations
    this->screenBuffer = new TFT_eSprite(&M5.Lcd);
    this->screenBuffer->createSprite(WIDTH, HEIGHT);
    this->screenBuffer->setTextColor(TEXTCOLOR, TEXTBACKGROUND);
    this->screenBuffer->setTextSize(TEXTSIZE);
  }

  void update(float* data, int size) {
    drawGraph(data, size);
    // drawText(data[size - 1]); // todo: figure out why this crashes?
    this->screenBuffer->pushSprite(0, 0);
  }
};

#endif

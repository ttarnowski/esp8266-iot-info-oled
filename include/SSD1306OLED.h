#pragma once

#include <Adafruit_SSD1306.h>
#include <OLED.h>
#include <SPI.h>
#include <Wire.h>

class SSD1306OLED : public OLED {
public:
  SSD1306OLED(Adafruit_SSD1306 *ssd1306) { this->ssd1306 = ssd1306; }

  void display_text(const char *text, Coords coords = {0, 0}, uint8_t size = 3,
                    bool reset = true) {
    if (reset) {
      this->ssd1306->clearDisplay();
    }
    this->ssd1306->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    this->ssd1306->setCursor(coords.x, coords.y);
    this->ssd1306->setTextSize(size);
    this->ssd1306->print(text);
    this->ssd1306->display();
  }

private:
  Adafruit_SSD1306 *ssd1306;
};
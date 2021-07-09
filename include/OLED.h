#pragma once

struct Coords {
  uint8_t x;
  uint8_t y;
};

class OLED {
public:
  virtual void display_text(const char *text, Coords coords = {0, 0},
                            uint8_t size = 3, bool reset = true);
};
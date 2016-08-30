#pragma once

#include <cstdint>

class Color
{
private:
  uint8_t red;
  uint8_t green;
  uint8_t blue;

public:
  Color():
    red(0), green(0), blue(0)
  {}
  
  Color(uint8_t r, uint8_t g, uint8_t b):
    red(r), green(g), blue(b)
  {}

  Color(const double m):
    red(UINT8_MAX *m), green(UINT8_MAX * m), blue(UINT8_MAX * m)
  {}
  
  uint8_t getRed() const{
    return red;
  }
  uint8_t getGreen() const{
    return green;
  }
  uint8_t getBlue() const{
    return blue;
  }

};

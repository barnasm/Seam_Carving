#pragma once
#include "color.h"
#include <string>

class Image
{
protected:
  std::size_t width, height;
public:
  std::size_t getWidth(){
    return width;
  }
  std::size_t getHeight(){
    return height;
  }
  //virtual void  openFromFile(const std::string filename) =0;
  virtual Color getPixelColor(int x, int y) =0;
  virtual void  setPixelColor(std::size_t x, std::size_t y, const Color&) =0;
  virtual void setSameSize(Image&) =0;  
//virtual Image& newSameSizeImage() =0;
//virtual  copyImage
};

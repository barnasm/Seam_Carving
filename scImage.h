#pragma once

#include "image.h"
#include <vector>
#include "scPixel.h"

class ScImage: public Image
{
private:
  std::vector< std::vector< ScPixel >>image;
public:
  ScImage(const std::size_t width, const std::size_t height)
    :image(height, std::vector<ScPixel>(width))
  {
    this->width = width;
    this->height = height;
  }

  ~ScImage(){}

  void setSameSize(std::size_t width, std::size_t height){
    this->width = width;
    this->height = height;

    this->image = std::vector< std::vector< ScPixel >>(height, std::vector<ScPixel>(width));
  }  

 
  Color getPixelColor(int x, int y)
  {
  x %= (int)width;
  y %= (int)height;

  if(x == -1)
    x = (int)width - 1;
  if(y == -1)
    y = (int)height - 1;

    return image[y][x].getColor();
  }
  void setPixelColor(std::size_t x, std::size_t y, const Color &color)
  {
    image[y][x].setColor(color);
  }

  bool isSeam(std::size_t x, std::size_t y){
    return image[y][x].nthSeam != 0;
  }
  std::size_t getSeanNum(std::size_t x, std::size_t y){
    return image[y][x].nthSeam;
  }
  
  std::vector< ScPixel >& operator [](std::size_t i){
    return image[i];
  }
  
};

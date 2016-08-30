#pragma once

#include "image.h"
#include <vector>
#include "scPixel.h"

class ScImage: public Image
{
private:
  std::vector< std::vector< ScPixel >>image;
  std::size_t widthOrg;
public:
  ScImage(const std::size_t width, const std::size_t height)
    :image(height, std::vector<ScPixel>(width))
  {
    this->width = this->widthOrg = width;
    this->height = height;
  }
  
  void setWidth(std::size_t width){
    this->width = width;
  }
  

  void setSameSize(Image &image){
    width = image.getWidth();
    height = image.getHeight();

    this->image = std::vector< std::vector< ScPixel >>(height, std::vector<ScPixel>(width));
  }  

  //Image& newSameSizeImage(){
    //return ScImage(width, height);
  //}

  Color getPixelColor(int x, int y)
  {
    //std::size_t xt;
    //for(xt = 0; x > 0; xt++){
    //  if(image[y][xt].nthSeam == 0 || image[y][xt].nthSeam > (widthOrg - width))
    //    x--;
    //}
#if 1
  x %= (int)widthOrg;
  y %= (int)height;

  if(x == -1)
    x = (int)widthOrg - 1;
  if(y == -1)
    y = (int)height - 1;
#endif  
    return image[y][x].getColor();
  }
  void setPixelColor(std::size_t x, std::size_t y, const Color &color)
  {
    image[y][x].setColor(color);
  }
  
  std::vector< ScPixel >& operator [](std::size_t i){
    return image[i];
  }
  
};

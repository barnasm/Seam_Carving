#pragma once

#include <iostream>
#include "image.h"
#include "scImage.h"
#include "pixelEnergy.h"
#include "energySimple.h"
#include "energyRandom.h"
#include "color.h"
#include <algorithm>
#include <memory>

class SeamCarving
{
private:
  Image *imageEnergy;
  Image *imageToDisplay; 
  std::unique_ptr<ScImage> scImage;
  //ScImage *scImage;
  std::shared_ptr<PixelEnergy> energy;
  std::vector<ScPixel*> seams;
  std::size_t currentWidth;

  void computeImageEnergy();
  void verticalSeam();
  void setNextSeamPx(int xt, int x, int y);
  ScPixel* getPixelAbove(int x, int y);
  void setNeighbours(std::size_t, std::size_t);
public:
  bool block;
  SeamCarving(){};
  SeamCarving(Image&, Image&, Image&, std::shared_ptr<PixelEnergy>&);

  void setEnergyProvider(std::shared_ptr<PixelEnergy> &energy){
    this->energy = energy;
    computeImageEnergy();
  }
  
  void sizeChanged(std::size_t width, std::size_t height);  
  void createImageToShow();

  ScPixel::enUnit computeEnergy(std::size_t, std::size_t);  
};

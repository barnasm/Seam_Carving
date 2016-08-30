#pragma once

#include "scPixel.h"
#include "image.h"

class PixelEnergy
{
public:
  ScPixel::enUnit MAX_ENERGY;
  virtual ScPixel::enUnit computePixelEnergy(std::size_t, std::size_t, Image&) =0;
};

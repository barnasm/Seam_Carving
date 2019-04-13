#pragma once

#include "pixelEnergy.h"

class EnergyRandom: public PixelEnergy
{
public:
  EnergyRandom(){
    MAX_ENERGY = 100000.0;
  }
  
  ScPixel::enUnit computePixelEnergy(std::size_t, std::size_t, Image&); 
};

#pragma once

#include "pixelEnergy.h"

class EnergySimple: public PixelEnergy
{
public:
  enum Version {BOTH, VERTICAL, HORIZONTAL};
private:
  Version version;
public:
  EnergySimple(){
    this->version = BOTH;
    MAX_ENERGY = 390150.0;
  }
  EnergySimple(Version version){
    this->version = version;
    MAX_ENERGY = 390150.0;
  }
  
  ScPixel::enUnit computePixelEnergy(std::size_t, std::size_t, Image&); 
};

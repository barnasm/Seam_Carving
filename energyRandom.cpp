#include "energyRandom.h"
#include <random>

ScPixel::enUnit
EnergyRandom::computePixelEnergy
(std::size_t x, std::size_t y, Image &image){
  std::random_device rd;
  std::default_random_engine gen(rd());
  std::uniform_real_distribution<ScPixel::enUnit> dist(0.0, MAX_ENERGY);
  return dist(gen);
}

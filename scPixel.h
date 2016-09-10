#pragma once

#include "color.h"
#include <vector>
#include <iostream>
#include <stdexcept>

static const std::string EnumStrings[] = { "LEFT", "RIGHT", "UP", "DOWN" };
class ScPixel
{
public:
  using enUnit = double;
  ScPixel *nextSeamPx = nullptr;
  int dir;
  std::size_t nthSeam = 0;
  std::size_t countSeams = 0;
  enum Direction{LEFT=0, RIGHT=1, UP=2, DOWN=3};

private:
  enUnit energy;
  Color color;
  std::vector<ScPixel*> neighbours;// = std::vector<ScPixel*>(4, this);

public:

  ScPixel():
    neighbours(std::vector<ScPixel*>(4, nullptr))
  {
    nthSeam = 0;
    countSeams = 0;
    nextSeamPx = nullptr;
  }

  ~ScPixel(){
    
  }
  void setEnergy(const enUnit &energy){
    this->energy = energy;
  }
  const enUnit getEnergy()const{
    return energy;
  }

  void setColor(const Color &color){
    this->color = color;
  }
  Color& getColor(){
    return color;
  }

  ScPixel* getNeighbour(Direction dir){
    //if(neighbours[dir] == nullptr)
	//throw std::out_of_range(EnumStrings[dir]);

    return neighbours[dir];
  }
  void setNeighbour(ScPixel *px, Direction dir){
    neighbours[dir] = px;
  }
  
};

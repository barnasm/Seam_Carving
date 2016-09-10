#include "energySimple.h"

ScPixel::enUnit
EnergySimple::computePixelEnergy
(std::size_t x, std::size_t y, Image &image){
  ScPixel::enUnit deltaX, deltaY;
  
  Color px1 = image.getPixelColor(x-1, y);
  Color px2 = image.getPixelColor(x+1, y);

  deltaX =
    (px1.getRed() - px2.getRed()) *
    (px1.getRed() - px2.getRed()) +
    (px1.getGreen() - px2.getGreen()) *
    (px1.getGreen() - px2.getGreen()) +
    (px1.getBlue() - px2.getBlue()) *
    (px1.getBlue() - px2.getBlue());

  px1 = image.getPixelColor(x, y-1);
  px2 = image.getPixelColor(x, y+1);
  deltaY =
    (px1.getRed() - px2.getRed()) *
    (px1.getRed() - px2.getRed()) +
    (px1.getGreen() - px2.getGreen()) *
    (px1.getGreen() - px2.getGreen()) +
    (px1.getBlue() - px2.getBlue()) *
    (px1.getBlue() - px2.getBlue());

  switch(version){
    case BOTH:
	return deltaX + deltaY;
    case VERTICAL:
	return deltaY;
    case HORIZONTAL:	
	return deltaX;
    default:
  	return deltaX + deltaY; 
  }
}

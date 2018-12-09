#include <iostream>
#include <map>
#include "imageManager.h"

/*
  main
*/
int main(int, char**){
  ImageManager<Image> im;
  im.openImage("images/img.bmp");
  std::cout << im.getEText() << std::endl;

  im.saveImage("img_new.jpg", Image());
  std::cout << im.getEText() << std::endl;

  return 0;
}


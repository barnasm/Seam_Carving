#include <iostream>
#include <map>
#include "imageManager.h"

/*
  main
*/
int main(int argc, char** args){
  ImageManager im;
  im.openImage("images/img2.bmp");
  std::cout << im.getEText() << std::endl;

  im.saveImage("img_new.jpg", Image());
  std::cout << im.getEText() << std::endl;

  return 0;
}

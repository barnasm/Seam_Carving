#include <iostream>
#include <string>
#include "imageManager.h"
#include "bhead.h"

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
//#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

/*
  main
*/
int main(int, char**){
  std::string imgPath = "images/img.bmp"; // why ""s doesnt work?
  auto newImgPath = std::string(imgPath).insert(imgPath.find(".bmp"), "_new");
  
  ImageGIL img;
  ImageManagerBoost im;
  img = im.openImage(imgPath).value_or(img);
  std::cout << im.getEText() << std::endl;

  using namespace boost::gil;
  ImageGIL img2( rgb8_image_t(100, 100) );
  resize_view(const_view(img.m_img), view(img2.m_img), bilinear_sampler());
  
  im.saveImage(newImgPath, img2);
  std::cout << im.getEText() << std::endl;




  return 0;
}


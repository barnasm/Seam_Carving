#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include "imageManager.h"
#include "bhead.h"

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
//#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

using namespace boost::gil;

template<typename T = uint8_t>
struct pixel_t{
  T r, g, b;

  T& operator[](int i){
    switch(i){
    case 0: return r;
    case 1: return g;
    case 2: return b;
    default: return b;
    }
  }

  const T& operator[](int i) const{
    switch(i){
    case 0: return r;
    case 1: return g;
    case 2: return b;
    default: return b;
    }
  }
};

template<typename T = pixel_t<>>
class ByteImgWrapper{
public:
  int height, width;
  std::vector<T> img_byte;
  ByteImgWrapper(int w, int h): height(h), width(w), img_byte(w*h) {}
  T& operator() (int x, int y){ return img_byte[y*width + x]; }
  const T& operator() (int x, int y) const { return img_byte[y*width + x]; }
};

void computeEnergy(const auto& src, auto& dst, auto& energyTable){
    
  for (int y=0; y<src.height; ++y)
    for (int x=0; x<src.width; ++x){ //<-- access to -1 elem!
      int energy =
	(src(x-1,y)[0] - src(x+1,y)[0]) * (src(x-1,y)[0] - src(x+1,y)[0]) +
	(src(x-1,y)[1] - src(x+1,y)[1]) * (src(x-1,y)[1] - src(x+1,y)[1]) +
	(src(x-1,y)[2] - src(x+1,y)[2]) * (src(x-1,y)[2] - src(x+1,y)[2]);
      
      dst(x,y)[0] = dst(x,y)[1] = dst(x,y)[2] = energy/(255*3);
      energyTable(x,y) = energy;
    }
}

void computeEnergySum(const auto& energyTable, auto& energyTableSum){
  assert(energyTable.width > 2);
  assert(energyTable.height > 2);
  
  energyTableSum.img_byte = energyTable.img_byte;

  auto& origin = energyTable;
  auto& res = energyTableSum;
  
  for(int y=1; y < res.height; ++y){
    auto x   = res.img_byte.begin() + res.width * y;
    auto xu0 = res.img_byte.begin() + res.width * (y-1);

    *x += *std::min_element(xu0, xu0+2);
    int i = 2;
    for(++x, ++xu0; i < res.width; ++x, ++xu0, i++)
      *x += *std::min_element(xu0-1, xu0+2);
    *x += *std::min_element(xu0-1, xu0+1);
  }
}

void findMinPath(const auto& energyTable, auto& energyTableSum, auto& img){
  assert(energyTable.width > 2);
  assert(energyTable.height > 2);

  auto& res = energyTableSum;

  auto y = res.img_byte.begin() + img.width * (img.height-1);
  auto x = std::min_element(y, y+img.width);
  *x *= -1;
  //auto off = std::distance(y, x);
  
  for(int i=1; i < img.height; i++){
    auto off = std::distance(y, x);    
    img(off, img.height-i)[0] = 255;
    y -= res.width;
    
     if(off == 0)
       x = std::min_element(y, y+2);
     else if(off == img.width-1)
       x = std::min_element(y+off-1, y+off+1);
     else
       x = std::min_element(y+off-1, y+off+2);
     *x *= -1;
  }
}

void img2bytes(const auto& src, auto& dst){
  for (int y=0; y<src.height(); ++y)
    for (int x=0; x<src.width(); ++x){
      dst(x,y)[0] = src(x,y)[0];
      dst(x,y)[1] = src(x,y)[1];
      dst(x,y)[2] = src(x,y)[2];
    }
}

void bytes2img(const auto& src, auto& dst){
  for (int y=0; y<dst.height(); ++y)
    for (int x=0; x<dst.width(); ++x){
      dst(x,y)[0] = src(x,y)[0]; 
      dst(x,y)[1] = src(x,y)[1]; 
      dst(x,y)[2] = src(x,y)[2]; 
    }  
}

/*
  main
*/
int main(int, char**){
  std::string imgPath = "images/img.bmp"; // why ""s doesnt work?
  auto ImgPathOut = std::string(imgPath).insert(imgPath.find(".bmp"), "_out");
  
  ImageGIL img_in, img_out;
  ImageManagerBoost im;
  img_in = im.openImage(imgPath).value_or(img_in);
  std::cout << "open image: " << im.getEText() << std::endl;


  ByteImgWrapper img_byte_in ( img_in.m_img.width(), img_in.m_img.height() );
  ByteImgWrapper img_byte_out( img_in.m_img.width(), img_in.m_img.height() );

  img2bytes(view(img_in.m_img), img_byte_in);


  using Energy_t = int32_t;
  ByteImgWrapper<Energy_t> energyTable(img_byte_in.width, img_byte_in.height);
  ByteImgWrapper<Energy_t> energyTableSum(img_byte_in.width, img_byte_in.height);

  computeEnergy(img_byte_in, img_byte_out, energyTable);
  computeEnergySum(energyTable, energyTableSum);
  findMinPath(energyTable, energyTableSum, img_byte_out);
  
  // for(int y = 0; y < 10; y++){
  //   for(int x = 0; x < 10; x++){
  //     std::cout << std::setw(6) << energyTable(x,y) << " | ";
  //   }
  //   std::cout << std::endl << std::string(9*10,'-') << std::endl;
  // } 
  // for(int y = 0; y < 10; y++){
  //  std::cout << std::endl << std::string(9*10,'-') << std::endl;
  //  for(int x = 0; x < 10; x++){
  //    std::cout << std::setw(6) << energyTableSum(x,y) << " | ";
  //   }
  // }

  

  img_out = ImageGIL( rgb8_image_t(img_byte_out.width, img_byte_out.height) );
  bytes2img(img_byte_out, view(img_out.m_img));

  im.saveImage(ImgPathOut, img_out);
  std::cout << "save image: " << im.getEText() << std::endl;

  return 0;
}



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

void computeEnergy(const auto& src, auto& energyTable){
    
  for (int y=0; y<src.height; ++y)
    for (int x=0; x<src.width; ++x){ //<-- access to -1 elem!
      int energy =
	(src(x-1,y)[0] - src(x+1,y)[0]) * (src(x-1,y)[0] - src(x+1,y)[0]) +
	(src(x-1,y)[1] - src(x+1,y)[1]) * (src(x-1,y)[1] - src(x+1,y)[1]) +
	(src(x-1,y)[2] - src(x+1,y)[2]) * (src(x-1,y)[2] - src(x+1,y)[2]);
      
      energyTable(x,y) = energy;
    }
}

void computeEnergySum(const auto& energyTable, auto& energyTableSum){
  assert(energyTable.width > 2);
  assert(energyTable.height > 2);
  
  energyTableSum.img_byte = energyTable.img_byte;

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

int findMin(const auto& energyTableSum, const auto& removedPixels, int x, int y){
  while(x < removedPixels.width && removedPixels(x,y)) x++;
  
  int min = x;
  for(int i = x; i < removedPixels.width; i++){
    if(not removedPixels(i,y) && energyTableSum(i,y) < energyTableSum(min,y))
      min = i;
  }
  return min;
}

void findMinPath(const auto& energyTable, auto& energyTableSum, auto& removedPixels){
  assert(energyTable.width > 2);
  assert(energyTable.height > 2);
  assert(energyTable.height == energyTableSum.height);
  assert(energyTable.height == removedPixels.height);
  assert(energyTable.width  == energyTableSum.width);
  assert(energyTable.width  == removedPixels.width);
  
  
  auto& res = energyTableSum;
    
  auto y = res.img_byte.begin() + res.width * (res.height-1);
  auto x = std::min_element(y, y+res.width);
  //*x |= 0xffffffff;
  auto min = findMin(energyTableSum, removedPixels, 0, res.height-1);
    
  for(int i=1; i < res.height; i++){
    auto off = std::distance(y, x);    
    removedPixels(off, res.height-i) = true;
    y -= res.width;
    
     if(off == 0)
       x = std::min_element(y, y+2);
     else if(off == res.width-1)
       x = std::min_element(y+off-1, y+off+1);
     else
       x = std::min_element(y+off-1, y+off+2);
     //*x |= 0xffffffff;
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

void visualiseSeams(const auto& removedPixels, auto& img, const auto& energyTable){
  for (int y=0; y<img.height; ++y)
    for (int x=0; x<img.width; ++x){
      img(x,y)[0] = img(x,y)[1] = img(x,y)[2] = energyTable(x,y)/(255*3);
      img(x,y).r |=  -removedPixels(x,y) & 0xff;
    }
}

void removeSeam(const auto& removedPixels, auto& img){
  ByteImgWrapper img_res(img.width-1, img.height);
  for (int y=0; y<img_res.height; ++y)
    for (int x=0, o=0; x<img_res.width; ++x){
      if(removedPixels(x,y)) o=1;
      img_res(x,y) = img(x+o, y);
    }
  img.img_byte.swap(img_res.img_byte);
  img.width--;
}

/*
  main
*/
int main(int, char**){
  std::string imgPath = "images/img2.bmp"; // why ""s doesnt work?
  auto ImgPathOut = std::string(imgPath).insert(imgPath.find(".bmp"), "_out");
  auto ImgEnergyPathOut = std::string(imgPath).insert(imgPath.find(".bmp"), "_energy_out");
  
  ImageGIL img_in, img_out, img_energy_out;
  ImageManagerBoost im;
  img_in = im.openImage(imgPath).value_or(img_in);
  std::cout << "open image: " << im.getEText() << std::endl;


  
  ByteImgWrapper img_byte ( img_in.m_img.width(), img_in.m_img.height() );
  ByteImgWrapper img_energy_byte_out( img_in.m_img.width(), img_in.m_img.height() );

  img2bytes(view(img_in.m_img), img_byte);


  for(int i = 0; i < img_in.m_img.width()/2; i++){
    using Energy_t = int32_t;
    ByteImgWrapper<Energy_t> energyTable   (img_byte.width, img_byte.height);
    ByteImgWrapper<Energy_t> energyTableSum(img_byte.width, img_byte.height);
    ByteImgWrapper<int8_t>   removedPixels (img_byte.width, img_byte.height);
    
    computeEnergy(img_byte, energyTable);
    computeEnergySum(energyTable, energyTableSum);
    findMinPath(energyTable, energyTableSum, removedPixels);
    //visualiseSeams(removedPixels, img_energy_byte_out, energyTable);
    removeSeam(removedPixels, img_byte); 
  }
  
  img_out = ImageGIL( rgb8_image_t(img_byte.width, img_byte.height) );
  bytes2img(img_byte, view(img_out.m_img));
  im.saveImage(ImgPathOut, img_out);
  std::cout << "save out image: " << im.getEText() << std::endl;

  // img_energy_out =ImageGIL( rgb8_image_t(img_energy_byte_out.width, img_energy_byte_out.height) );
  // bytes2img(img_energy_byte_out, view(img_energy_out.m_img));
  // im.saveImage(ImgEnergyPathOut, img_energy_out);
  // std::cout << "save energy image: " << im.getEText() << std::endl;

  
  return 0;
}



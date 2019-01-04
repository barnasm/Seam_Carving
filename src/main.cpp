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

template<typename T = int32_t>
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
    for (int x=0; x<src.width; ++x){
      std::cout << src(x-1,y)[0] << " " << src(x-1,y)[1] << " " << src(x-1,y)[2] << std::endl;
      int32_t energy =
  	  (src(x-1,y)[0] - src(x+1,y)[0]) * (src(x-1,y)[0] - src(x+1,0)[0])
  	+ (src(x-1,y)[1] - src(x+1,y)[1]) * (src(x-1,y)[1] - src(x+1,0)[1]) 
  	+ (src(x-1,y)[2] - src(x+1,y)[2]) * (src(x-1,y)[2] - src(x+1,0)[2]);
      
      // dst(x,y)[0] = dst(x,y)[1] = 
	dst(x,y)[2] = energy/(255*3);
      // dst(x,y)[0] = src(x,y)[0];
      // dst(x,y)[1] = src(x,y)[1];
      // dst(x,y)[2] = src(x,y)[2];
      energyTable(x,y) = energy;
    }
}

void computeEnergySum(const auto& energyTable, auto& energyTableSum){
  // assert(energyTable.size() > 2);
  // assert(energyTable[0].size() > 2);
  
  // energyTableSum = energyTable;
  // auto& res = energyTableSum;
  
  // //res[0] = energyTable[0];
  // for(auto yu = res.begin(), y = res.begin()+1; y < res.end(); ++y, ++yu){
  //   auto x = y->begin();
  //   auto xu0 = yu->begin();

  //   *x += *std::min_element(xu0, xu0+2);
  //   for(++x, ++xu0; x < y->end()-1; ++x, ++xu0)
  //     *x += *std::min_element(xu0-1, xu0+2);
  //   *x += *std::min_element(xu0-1, xu0+1);
  // }
}

void findMinPath(const auto& energyTable, auto& energyTableSum, ImageGIL<>& img4){
  // assert(energyTable.size() > 2);
  // assert(energyTable[0].size() > 2);

  // auto src = view(img4.m_img);
  // rgb8c_view_t::xy_locator simg = src.xy_at(0,0);
  // // simg[0] = simg[1] = simg[2] = 0;
  // // simg[0] = 255;

  // auto& res = energyTableSum;

  // auto y = res.rbegin();
  // auto x = std::min_element(y->begin(), y->end());
  // *x *= -1;
  
  // for(; y < res.rend()-1;){
  //   auto off = std::distance(y->begin(), x);    
  //   ++y;
    
  //   if(off == 0)
  //     x = std::min_element(y->begin()+off, y->begin()+off+2);
  //   else if(off == y->size()-1)
  //     x = std::min_element(y->begin()+off-1, y->begin()+off+1);
  //   else
  //     x = std::min_element(y->begin()+off-1, y->begin()+off+2);
  //   *x *= -1;
  // }
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
  std::string imgPath = "images/10x10.bmp"; // why ""s doesnt work?
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
  //computeEnergySum(energyTable, energyTableSum);

  for(int y = 0; y < 10; y++){
    for(int x = 0; x < 10; x++){
      std::cout << std::setw(6) << energyTable(x,y) << " | ";
    }
    std::cout << std::endl << std::string(9*10,'-') << std::endl;
  } 

  

  img_out = ImageGIL( rgb8_image_t(img_byte_out.width, img_byte_out.height) );
  bytes2img(img_byte_out, view(img_out.m_img));

  im.saveImage(ImgPathOut, img_out);
  std::cout << "save image: " << im.getEText() << std::endl;

  return 0;




  
#if 0
  ImageGIL img4(rgb8_image_t(img.m_img.dimensions()));
  computeEnergy(img, img4, energyTable);
  computeEnergySum(energyTable, energyTableSum);


  findMinPath(energyTable, energyTableSum, img4);

  for(int y = 0; y < 10; y++){
    for(int x = 0; x < 10; x++){
      std::cout << std::setw(6) << energyTable[y][x] << " | ";
    }
    std::cout << std::endl << std::string(9*10,'-') << std::endl;
  } 
  for(int y = 0; y < 10; y++){
   std::cout << std::endl << std::string(9*10,'-') << std::endl;
   for(int x = 0; x < 10; x++){
      std::cout << std::setw(6) << energyTableSum[y][x] << " | ";
    }
  }
 #endif
}



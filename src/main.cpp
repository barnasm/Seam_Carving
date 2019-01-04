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

typedef struct {
  uint8_t r, g, b;
}pixel_t;

class ByteImgWrapper{
public:
  int heigh, width;
  std::vector<pixel_t> img_byte;
  ByteImgWrapper(int w, int h): heigh(h), width(w), img_byte(w*h) {}
  pixel_t& operator() (int x, int y){ return img_byte[y*width + x]; }
  const pixel_t& operator() (int x, int y) const { return img_byte[y*width + x]; }
};

void computeEnergy(const auto src[], auto dst[], auto& energyTable, int height, int width){
    
  // for (int y=0; y<height; ++y)
  //   for (int x=0; x<width; ++x){
  //     auto energy =
  // 	  (src(x-1,y)[0] - src(x+1,y)[0]) * (src(x-1,y)[0] - src(x+1,0)[0])
  // 	+ (src(x-1,y)[1] - src(x+1,y)[1]) * (src(x-1,y)[1] - src(x+1,0)[1]) 
  // 	+ (src(x-1,y)[2] - src(x+1,y)[2]) * (src(x-1,y)[2] - src(x+1,0)[2]);
      
  //     dst(x,y)[0] = dst(x,y)[1] = dst(x,y)[2] = energy/(255*3);
  //     energyTable[y][x] = energy;
  //   }
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
      dst(x,y).r = src(x,y)[0];
      dst(x,y).g = src(x,y)[1];
      dst(x,y).b = src(x,y)[2];
    }
}

void bytes2img(const auto& src, auto& dst){
  for (int y=0; y<dst.height(); ++y)
    for (int x=0; x<dst.width(); ++x){
      dst(x,y)[0] = src(x,y).r; 
      dst(x,y)[1] = src(x,y).g; 
      dst(x,y)[2] = src(x,y).b; 
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


  
  ByteImgWrapper img_byte( img_in.m_img.width(), img_in.m_img.height() );

  img2bytes(view(img_in.m_img), img_byte);
  img_out = ImageGIL( rgb8_image_t(img_in.m_img.width()/2, img_in.m_img.height()) );
  bytes2img(img_byte, view(img_out.m_img));

  //im.saveImage(ImgPathOut, img_out);
  //std::cout << "save image: " << im.getEText() << std::endl;

  return 0;




  
#if 0
  using Energy_t = int32_t;
  
  std::vector< std::vector<Energy_t> > energyTable   (img.m_img.height(), std::vector<Energy_t>(img.m_img.width()));
  std::vector< std::vector<Energy_t> > energyTableSum(img.m_img.height(), std::vector<Energy_t>(img.m_img.width()));
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
 
  im.saveImage(newImgPath, img4);
  std::cout << im.getEText() << std::endl;

  return 0;
#endif
}



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
void computeEnergy(ImageGIL<>& img, ImageGIL<>& img4, auto& energyTable){
  
  auto src = view(img.m_img);
  auto dst = view(img4.m_img);
  
  rgb8c_view_t::xy_locator src_loc = src.xy_at(0,0);
  for (int y=0; y<src.height(); ++y)
    {
      rgb8_view_t::x_iterator dst_it  = dst.row_begin(y);
      
       for (int x=0; x<src.width(); ++x)
	 {
	   auto energy =
	     (src_loc(-1,0)[0] - src_loc(1,0)[0])   * (src_loc(-1,0)[0] - src_loc(1,0)[0])
	     + (src_loc(-1,0)[1] - src_loc(1,0)[1]) * (src_loc(-1,0)[1] - src_loc(1,0)[1]) 
	     + (src_loc(-1,0)[2] - src_loc(1,0)[2]) * (src_loc(-1,0)[2] - src_loc(1,0)[2]);
	     
	   (*dst_it)[0] = (*dst_it)[1] = (*dst_it)[2] = energy/(255*3);
	   energyTable[y][x] = energy;
          ++dst_it;
          ++src_loc.x(); // each dimension can be advanced separately
	 }
      src_loc+=point2<std::ptrdiff_t>(-src.width(),1); // carriage return
    }
}

void computeEnergySum(const auto& energyTable, auto& energyTableSum){
  assert(energyTable.size() > 2);
  assert(energyTable[0].size() > 2);
  
  energyTableSum = energyTable;
  auto& res = energyTableSum;
  
  //res[0] = energyTable[0];
  for(auto yu = res.begin(), y = res.begin()+1; y < res.end(); ++y, ++yu){
    auto x = y->begin();
    auto xu0 = yu->begin();

    *x += *std::min_element(xu0, xu0+2);
    for(++x, ++xu0; x < y->end()-1; ++x, ++xu0)
      *x += *std::min_element(xu0-1, xu0+2);
    *x += *std::min_element(xu0-1, xu0+1);
  }
}

void findMinPath(const auto& energyTable, auto& energyTableSum, ImageGIL<>& img4){
  assert(energyTable.size() > 2);
  assert(energyTable[0].size() > 2);

  auto src = view(img4.m_img);
  rgb8c_view_t::xy_locator simg = src.xy_at(0,0);
  simg[0] = simg[1] = simg[2] = 0;
  simg[0] = 255;

  auto& res = energyTableSum;

  auto y = res.rbegin();
  auto x = std::min_element(y->begin(), y->end());
  *x *= -1;
  
  for(; y < res.rend()-1;){
    auto off = std::distance(y->begin(), x);    
    ++y;
    
    if(off == 0)
      x = std::min_element(y->begin()+off, y->begin()+off+2);
    else if(off == y->size()-1)
      x = std::min_element(y->begin()+off-1, y->begin()+off+1);
    else
      x = std::min_element(y->begin()+off-1, y->begin()+off+2);
    *x *= -1;
  }
}



// ImageGIL img3(rgb8_image_t(img.m_img.dimensions()));
  // using Img = rgb8_image_t;
  // using Pix = Img::value_type;
  // auto foo =
  //   [](rgb8_ref_t a) {
  //   red_t   R;
  //   green_t G;
  //   blue_t  B;
  //   //alpha_t A;
  //   return Pix (
  // 		get_color(a, R) ^ (get_color(a, R) | ~get_color(a, R)),
  // 		get_color(a, G) ^ (get_color(a, G) | ~get_color(a, G)),
  // 		get_color(a, B) ^ (get_color(a, B) | ~get_color(a, B))
		
  // 		//get_color(a, G) + get_color(a, G),
  // 		//get_color(a, B) + get_color(a, B)
  // 		//get_color(a, A) + get_color(a, A)
  //               );
  // };

  // transform_pixels(view(img.m_img), view(img3.m_img), foo);



/*
  main
*/
int main(int, char**){
  std::string imgPath = "images/10x10.bmp"; // why ""s doesnt work?
  auto newImgPath = std::string(imgPath).insert(imgPath.find(".bmp"), "_new");
  
  ImageGIL img;
  ImageManagerBoost im;
  img = im.openImage(imgPath).value_or(img);
  std::cout << im.getEText() << std::endl;

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
}


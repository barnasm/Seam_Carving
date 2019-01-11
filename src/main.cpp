

#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include <chrono>
#include <ctime>
#include "imageManager.h"
#include "bhead.h"
#include <unistd.h>

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
//#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

using namespace boost::gil;


using pxBase = uint8_t;
template<typename T = pxBase>
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
  ByteImgWrapper(int w, int h): height(h), width(w), img_byte(w*h+1) {}
  T& operator() (int x, int y){ return img_byte[y*width + x]; }
  const T& operator() (int x, int y) const { return img_byte[y*width + x]; }
};

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
  Seam carving
*/

void computeEnergy(const auto& src, auto& energyTable){
    
  for (int y=0; y<src.height; ++y)
    for (int x=0; x<src.width; ++x){ //<-- access to -1 elem!
      int energy =
	(src(x-1,y).r - src(x+1,y).r) * (src(x-1,y).r - src(x+1,y).r) +
	(src(x-1,y).g - src(x+1,y).g) * (src(x-1,y).g - src(x+1,y).g) +
	(src(x-1,y).b - src(x+1,y).b) * (src(x-1,y).b - src(x+1,y).b);
      
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

// int findMin(const auto& energyTableSum, const auto& removedPixels, int x, int y){
//   while(x < removedPixels.width && removedPixels(x,y)) x++;
  
//   int min = x;
//   for(int i = x; i < removedPixels.width; i++){
//     if(not removedPixels(i,y) && energyTableSum(i,y) < energyTableSum(min,y))
//       min = i;
//   }
//   return min;
// }

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
  //auto min = findMin(energyTableSum, removedPixels, 0, res.height-1);
    
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
  auto off = std::distance(y, x);    
  removedPixels(off, 0) = true;
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

void SeamCarving(const auto& img_in, auto& img_byte, auto& img_energy_byte_out, int n = 1){
  using Energy_t = int32_t;
  
  ByteImgWrapper<Energy_t> energyTable   (img_byte.width, img_byte.height);
  ByteImgWrapper<Energy_t> energyTableSum(img_byte.width, img_byte.height);

  for(int i = 0; i < n; i++){
    ByteImgWrapper<int8_t>   removedPixels (img_byte.width, img_byte.height);
    
    computeEnergy(img_byte, energyTable);
    computeEnergySum(energyTable, energyTableSum);
    findMinPath(energyTable, energyTableSum, removedPixels);
    //visualiseSeams(removedPixels, img_energy_byte_out, energyTable);
    removeSeam(removedPixels, img_byte); 

    energyTable.width--;
    energyTableSum.width--;
    
    // for(int y = 0; y < energyTable.height; y++){
    //   for(int x = 0; x < energyTable.width; x++)
    // 	std::cout << std::setw(8) << (int)removedPixels(x,y);
    //   std::cout << std::endl;
    // }  std::cout << std::endl;
    
  }
}

void timeMeasure(auto& foo){
  std::clock_t c_start = std::clock();
  auto t_start = std::chrono::high_resolution_clock::now();

  foo();

  std::clock_t c_end = std::clock();
  auto t_end = std::chrono::high_resolution_clock::now();
 
  std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
	    << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
	    << "Wall clock time passed: "
	    << std::chrono::duration<double, std::milli>(t_end-t_start).count()
	    << " ms\n\n";
}

extern "C" void cudaProxy(pxBase* img, pxBase* img_res, int64_t w, int64_t h, int64_t N);


/*
  main
*/
int main(int argc, char** argv){
  int N = 0;
  std::string imgPath = "images/"; // why ""s doesnt work?

  int c;
  while ((c = getopt (argc, argv, "f:s:")) != -1)
    switch (c)
      {
      case 'f':
        imgPath += optarg;
        break;
      case 's':
        N = atol(optarg);
        break;
      case '?':
        if (optopt == 'f' || optopt == 's')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
  std::cout << "seam carving on: " << imgPath << ", remove: " << N << " pixels.\n";
  
  auto ImgPathOut = std::string(imgPath).insert(imgPath.find(".bmp"), "_out");
  auto ImgPathCudaOut = std::string(imgPath).insert(imgPath.find(".bmp"), "_cuda_out");
  auto ImgEnergyPathOut = std::string(imgPath).insert(imgPath.find(".bmp"), "_energy_out");
  
  ImageGIL img_in, img_out, img_energy_out;
  ImageManagerBoost im;
  img_in = im.openImage(imgPath).value_or(img_in);
  //std::cout << "open image: " << im.getEText() << std::endl;

  
  ByteImgWrapper img_byte ( img_in.m_img.width(), img_in.m_img.height() );
  ByteImgWrapper img_energy_byte_out( img_in.m_img.width(), img_in.m_img.height() );

  img2bytes(view(img_in.m_img), img_byte);
  
  auto foo = [&]{ SeamCarving(img_in, img_byte, img_energy_byte_out, N); };
  timeMeasure( foo );
  
  img_out = ImageGIL( rgb8_image_t(img_byte.width, img_byte.height) );
  bytes2img(img_byte, view(img_out.m_img));
  im.saveImage(ImgPathOut, img_out);
  //std::cout << "save out image: " << im.getEText() << std::endl;
  
  ByteImgWrapper img_byte2 ( img_in.m_img.width(), img_in.m_img.height() );
  ByteImgWrapper img_byte2_res ( img_in.m_img.width()-N, img_in.m_img.height() );
  img2bytes(view(img_in.m_img), img_byte2);
  
  uint8_t* imgp = reinterpret_cast<uint8_t*>(img_byte2.img_byte.data());
  uint8_t* imgp_res = reinterpret_cast<uint8_t*>(img_byte2_res.img_byte.data());
  
  auto bar = [&]{ cudaProxy(imgp, imgp_res, img_byte2.width, img_byte2.height, N); };
  timeMeasure(bar);

  bytes2img(img_byte2_res, view(img_out.m_img));
  im.saveImage(ImgPathCudaOut, img_out);  
  return 0;
}



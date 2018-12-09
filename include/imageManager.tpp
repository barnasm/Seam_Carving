#pragma once
//only for .cpp #include "imageManager.h"

template<typename Img>
ImageManager<Img>::ImageManager(){
  eMap[ECodes::cantOpen] = "Could not open image.";
  eMap[ECodes::cantSave] = "Could not save image.";
}

template<typename Img>
typename ImageManager<Img>::OptImage
ImageManager<Img>::openImage(const std::string path) try{
  boost::gil::rgb8_image_t img;
  read_and_convert_image( path, img, boost::gil::bmp_tag() );
  return Image();
 }
 catch(...){
  setECode(ECodes::cantOpen);
  return std::nullopt;   
 }

template<typename Img>
typename ImageManager<Img>::OptImage
ImageManager<Img>::saveImage(const Img &img, const std::string path){
  setECode(ECodes::cantSave);
  return std::nullopt;
}
template<typename Img>
typename ImageManager<Img>::OptImage
ImageManager<Img>::saveImage(const std::string path,const Img &img)
{
  return saveImage(img, path);
}



//if it is .cpp file here put types:
//template class ImageManager<ImageGIL>;

//if it is .tpp file include this file at the end of header file 


#pragma once
//only for .cpp #include "imageManager.h"

template<typename Img>
ImageManagerBoost<Img>::ImageManagerBoost(){
  eMap[ECodes::cantOpen] = "Could not open image.";
  eMap[ECodes::cantSave] = "Could not save image.";
}

template<typename Img>
typename ImageManagerBoost<Img>::OptImage
ImageManagerBoost<Img>::openImage(const std::string path) try{
  Img img;
  read_and_convert_image( path, static_cast<ImageIplR>(img), boost::gil::bmp_tag() );
  return Img(std::move(img));
 }
 catch(...){
  setECode(ECodes::cantOpen);
  return std::nullopt;   
 }

template<typename Img>
typename ImageManagerBoost<Img>::OptImage
ImageManagerBoost<Img>::saveImage(const Img &img, const std::string path) try{
  Img img_tmp(img); 
  write_view(path, view(static_cast<ImageIplR>(img_tmp)), boost::gil::bmp_tag());
  return img;
 }
 catch(...){
   setECode(ECodes::cantSave);
   return std::nullopt;
 }
template<typename Img>
typename ImageManagerBoost<Img>::OptImage
ImageManagerBoost<Img>::saveImage(const std::string path,const Img &img)
{
  return saveImage(img, path);
}

//if it is .cpp file here put types:
//template class ImageManagerBoost<ImageGIL>;

//if it is .tpp file include this file at the end of header file 


#include "imageManager.h"


OptImage ImageManager::openImage(const std::string path) try{
  boost::gil::rgb8_image_t img;
  read_and_convert_image( path, img, boost::gil::bmp_tag() );
  return Image();
 }
 catch(...){
  setECode(ECodes::cantOpen);
  return std::nullopt;   
 }

OptImage ImageManager::saveImage(const Image &img, const std::string path){
  setECode(ECodes::cantSave);
  return std::nullopt;
}
OptImage ImageManager::saveImage(const std::string path,const Image &img)
{
  return saveImage(img, path);
}

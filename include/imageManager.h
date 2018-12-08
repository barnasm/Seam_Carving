#pragma once

#include "image.h"
#include "ierror.h"
#include "boost/gil.hpp"
#include "boost/gil/extension/io/bmp.hpp"

/*
  ImageManager
*/
enum class ECodes{noError, cantOpen, cantSave};
using OptImage = std::optional<Image>;
class ImageManager : public IError<ECodes>{
public:
  ImageManager(){
    eMap[ECodes::cantOpen] = "Could not open image.";
    eMap[ECodes::cantSave] = "Could not save image.";
  }
  
  OptImage openImage(const std::string path);
  OptImage saveImage(const Image &img, const std::string path);
  OptImage saveImage(const std::string path, const Image &img);
};

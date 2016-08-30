#pragma once

#include <iostream>
#include <gtkmm.h>
#include <string>
#include "image.h"
#include "color.h"

class ImageByGtkmm: public Image
{
private:
  bool changed = false;
  guint8* getPixel(int x, int y);
  Glib::RefPtr<Gdk::Pixbuf> imageOriginal;
  Glib::RefPtr<Gdk::Pixbuf> *imgPtr;
  
public:
  ImageByGtkmm( Glib::RefPtr<Gdk::Pixbuf> &image)
  {
    imgPtr = &image;
  }
  void setSameSize(Image&);
  void openFromFile(const std::string filename);
  Color getPixelColor(int x, int y);
  void setPixelColor(std::size_t x, std::size_t y, const Color&);
  
  void fitToWindow(std::size_t width, std::size_t height){
    if(!imageOriginal)
      return;
    

    double d = std::min((double)width / (double)this->width, (double)height / (double)this->height);
    if((!changed && (int)(this->width * d) == (*imgPtr)->get_width()) || d <= 0)
      return;

    changed = false;  
  
    *imgPtr = imageOriginal->scale_simple((int)(this->width  * d),
					  (int)(this->height * d),
					  Gdk::INTERP_BILINEAR);
  }
};

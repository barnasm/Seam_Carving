#include "imageByGtkmm.h"
#include <iostream> 

void ImageByGtkmm::openFromFile(const std::string filename)
{
  try{
    imageOriginal = *imgPtr = Gdk::Pixbuf::create_from_file(filename);
    width  = imageOriginal->get_width();
    height = imageOriginal->get_height();
  }
  catch(const Gio::ResourceError& ex){
    std::cerr << "ResourceError: " << ex.what() << std::endl;
  }
  catch(const Gdk::PixbufError& ex){
    std::cerr << "PixbufError: " << ex.what() << std::endl;
  }
  changed = true;
}

void ImageByGtkmm::setSameSize(Image &img)
{
  width = img.getWidth();
  height = img.getHeight();
  imageOriginal = *imgPtr = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,
						false,
      						8,
						width,
						height);
  changed = true;
}


guint8* ImageByGtkmm::getPixel(int x, int y){
  //n_channels = imageOriginal->get_n_channels();
  
  g_assert (imageOriginal->get_colorspace() == Gdk::COLORSPACE_RGB);
  g_assert (imageOriginal->get_bits_per_sample() == 8);
  //g_assert (m_image_tmp->get_has_alpha());
  //g_assert (n_channels == 4);

#if 1
  x %= (int)width;
  y %= (int)height;

  if(x == -1)
    x = (int)width - 1;
  if(y == -1)
    y = (int)height - 1;
#endif  
  g_assert (x >= 0 && x < (int)width);
  g_assert (y >= 0 && y < (int)height);

  guint8 *px;
  px =  imageOriginal->get_pixels() +
    y * imageOriginal->get_rowstride() +
    x * imageOriginal->get_n_channels();
  
  return px;
}

Color ImageByGtkmm::getPixelColor(int x, int y){
  auto px = getPixel(x, y); 
  return Color(px[0], px[1], px[2]);
}

void ImageByGtkmm::setPixelColor(std::size_t x, std::size_t y, const Color &col){
  auto px = getPixel((int)x, (int)y); 
  px[0] = col.getRed();
  px[1] = col.getGreen();
  px[2] = col.getBlue();
  changed = true;
}



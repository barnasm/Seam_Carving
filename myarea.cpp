/* gtkmm example Copyright (C) 2011 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "myarea.h"
#include <cairomm/context.h>
#include <giomm/resource.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <glibmm/fileutils.h>
#include <iostream>
#include <gtk/gtk.h>

MyArea::MyArea()
{
  try
  {
    // The fractal image has been created by the XaoS program.
    // http://xaos.sourceforge.net
    m_image_org = Gdk::Pixbuf::create_from_file("image/test5.jpg");
    m_image_cpy = m_image_org->copy();
    for(int i = 0; i < 140; i++){
      imageEnergy = imageEnergyVisualization(m_image_cpy);
      findVerticalSeam();
    }
  }
  catch(const Gio::ResourceError& ex)
  {
    std::cerr << "ResourceError: " << ex.what() << std::endl;
  }
  catch(const Gdk::PixbufError& ex)
  {
    std::cerr << "PixbufError: " << ex.what() << std::endl;
  }

  // Show at least a quarter of the image.
  if (m_image_org)
    set_size_request(10, 10);
}

MyArea::~MyArea()
{
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  if (!m_image_cpy)
    return false;

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  //auto crClear = gdk_cairo_create (Gtk::DrawingArea::get_window());
  auto crClear = cairo_create(cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height));
  cairo_set_operator (crClear, CAIRO_OPERATOR_CLEAR);
  cairo_paint (crClear);
  cairo_destroy (crClear);
  
  // Draw the image in the middle of the drawing area, or (if the image is
  // larger than the drawing area) draw the middle part of the image.

  
  Gdk::Cairo::set_source_pixbuf(cr, m_image_org,
				(width - m_image_org->get_width())/2,
				0);
  cr->paint();
  Gdk::Cairo::set_source_pixbuf(cr, imageEnergy,
  				(width - imageEnergy->get_width())/2,
  				imageEnergy->get_height());
  cr->paint();
  
  Gdk::Cairo::set_source_pixbuf(cr, m_image_cpy,
				(width - m_image_cpy->get_width())/2,
				m_image_org->get_height() * 2);
  
  cr->paint();
    
  return true;
}

guint8* getPixelColor(const Glib::RefPtr<Gdk::Pixbuf> image, int x, int y){
  int width, height, rowstride, n_channels;
  guint8 *pixels;
  guint8 *px;
  
  n_channels = image->get_n_channels();
  
  g_assert (image->get_colorspace() == GDK_COLORSPACE_RGB);
  g_assert (image->get_bits_per_sample() == 8);
  //g_assert (m_image_tmp->get_has_alpha());
  //g_assert (n_channels == 4);

  width  = image->get_width ();
  height = image->get_height();

  x %= width;
  y %= height;

  if(x < 0)
    x = width - 1;
  if(y < 0)
    y = height - 1;
  
  g_assert (x >= 0 && x < width);
  g_assert (y >= 0 && y < height);

  rowstride = image->get_rowstride();
  pixels    = image->get_pixels();

  px = pixels + y * rowstride + x * n_channels;

  //std::cout << x << " " << y << "  r:" << (int)px[0] << " g:" << (int)px[1] << " b:" << (int)px[2] << std::endl;

  return px;
}

void setPixelColor(Glib::RefPtr<Gdk::Pixbuf> image, int x, int y, guint8 r, guint8 g, guint8 b){
  int width, height, rowstride, n_channels;
  guint8 *pixels;
  guint8 *px;
  
  n_channels = image->get_n_channels();
  
  g_assert (image->get_colorspace() == GDK_COLORSPACE_RGB);
  g_assert (image->get_bits_per_sample() == 8);
  //g_assert (m_image_tmp->get_has_alpha());
  //g_assert (n_channels == 4);

  width  = image->get_width ();
  height = image->get_height();

  x %= width;
  y %= height;

  if(x < 0)
    x = width - 1;
  if(y < 0)
    y = height - 1;
  
  g_assert (x >= 0 && x < width);
  g_assert (y >= 0 && y < height);

  rowstride = image->get_rowstride();
  pixels    = image->get_pixels();

  px = pixels + y * rowstride + x * n_channels;

  //std::cout << "r:" << (int)px[0] << " g:" << (int)px[1] << " b:" << (int)px[2] << std::endl;
  px[0] = r;
  px[1] = g;
  px[2] = b;
}

int MyArea::computePixelEnergy(const Glib::RefPtr<Gdk::Pixbuf> image, int x, int y){
  int deltaX, deltaY;

  auto px1 = getPixelColor(image, x-1, y);
  auto px2 = getPixelColor(image, x+1, y);
  deltaX = (px1[0] - px2[0]) * (px1[0] - px2[0]) +
           (px1[1] - px2[1]) * (px1[1] - px2[1]) +
           (px1[2] - px2[2]) * (px1[2] - px2[2]);

  px1 = getPixelColor(image, x, y-1);
  px2 = getPixelColor(image, x, y+1);
  deltaY = (px1[0] - px2[0]) * (px1[0] - px2[0]) +
           (px1[1] - px2[1]) * (px1[1] - px2[1]) +
           (px1[2] - px2[2]) * (px1[2] - px2[2]);

  return deltaX + deltaY;
  
}

Glib::RefPtr<Gdk::Pixbuf> MyArea::imageEnergyVisualization(Glib::RefPtr<Gdk::Pixbuf> image){

  int width = image->get_width(),
    height = image->get_height();
  std::cout << width << std::endl;
  
  Glib::RefPtr<Gdk::Pixbuf> energyPreview = Gdk::Pixbuf::create(image->get_colorspace(),
								image->get_has_alpha(),
								image->get_bits_per_sample(),
								width,
								height);

  // std::vector< std::vector<long long int> > vec2d(height, std::vector<long long int>(width));
  pixelsEnergy = std::make_shared< std::vector< std::vector<long long int>> >
    (std::vector<std::vector<long long int>>(height, std::vector<long long int>(width)));
  
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      (*pixelsEnergy)[y][x] = computePixelEnergy(image, x, y);
      guint8 pxCol = 1 * 255 *  (*pixelsEnergy)[y][x] / 390150;
      setPixelColor(energyPreview, x, y, pxCol, pxCol, pxCol);
      //std::cout << (*pixelsEnergy)[y][x] << "  ";
    }
    //std::cout << std::endl;
  }
  
  return energyPreview;
}

void MyArea::fitToWindow(){
  // Gtk::Allocation allocation = get_allocation();
  // m_image_cpy = m_image_org->scale_simple(allocation.get_width(),
  // 					  allocation.get_height(),
  // 					  Gdk::INTERP_BILINEAR);
    
}

std::vector<int> MyArea::findVerticalSeam(){
  int height = m_image_cpy->get_height();
  int width  = m_image_cpy->get_width();

  std::vector<std::vector<int>> paths(height, std::vector<int>(width, 0));

  std::shared_ptr<std::vector<long long int>> totalEnergy   = std::make_shared<std::vector<long long int>>((*pixelsEnergy)[0]);
  std::shared_ptr<std::vector<long long int>> totalEnergyTmp = std::make_shared<std::vector<long long int>>((*pixelsEnergy)[0]);

  for(int y = 1; y < height; y++){
    auto minEmem = std::min_element((*totalEnergy).begin(),
    				    (*totalEnergy).begin()+2);

    (*totalEnergyTmp)[0] = (*pixelsEnergy)[y][0] + *minEmem;
    paths[y][0] = minEmem - (*totalEnergy).begin();

    for(int x = 1; x < width-1; x++){
      minEmem = std::min_element((*totalEnergy).begin()+x-1,
				 (*totalEnergy).begin()+x+2);

      (*totalEnergyTmp)[x] = (*pixelsEnergy)[y][x] + *minEmem;
      paths[y][x] = minEmem - (*totalEnergy).begin();
    }
    
    minEmem = std::min_element((*totalEnergy).end()-2,
			       (*totalEnergy).end());

    (*totalEnergyTmp)[width-1] = (*pixelsEnergy)[y][0] + *minEmem;
    paths[y][width-1] = minEmem - (*totalEnergy).begin();
      
    std::swap(totalEnergy, totalEnergyTmp);
  }

#if 0
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++)
      std::cout << paths[y][x] << " ";
    std::cout << std::endl;
  }
#endif


  auto minEmem = std::min_element((*totalEnergy).begin(), (*totalEnergy).end());
  *minEmem = LLONG_MAX; 
  int x = minEmem - (*totalEnergy).begin();
  for(int y = height - 1; y >= 0; y--){
    setPixelColor(imageEnergy, x, y, 255, 0, 0);
    for(int xx = x; xx < width-1; xx++){
      auto px = getPixelColor(m_image_cpy, xx+1, y);
      setPixelColor(m_image_cpy, xx, y, px[0], px[1], px[2]);
    }
    x = paths[y][x];
  }
  m_image_tmp = m_image_cpy->copy();
  m_image_cpy = Gdk::Pixbuf::create(m_image_org->get_colorspace(),
				    m_image_org->get_has_alpha(),
				    m_image_org->get_bits_per_sample(),
				    width-1,
				    height);

  m_image_tmp->copy_area(0, 0, width-1, height, m_image_cpy, 0, 0);
  // m_image_cpy->save("5x5", "png");
  
  return {};
}

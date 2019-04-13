#include "myArea.h"
#include <cairomm/context.h>
#include <giomm/resource.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <glibmm/fileutils.h>
#include <iostream>
#include <gtk/gtk.h>

MyArea::MyArea()
{
  set_size_request(2, 2);
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  if (!image)
    return false;

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  
  Gdk::Cairo::set_source_pixbuf(cr, image,
  				(width  - image->get_width()) /2,
  				(height - image->get_height())/2);
  
  cr->paint();
  //Cairo::Context::create(surface)->paint();
  return true;
}

bool MyArea::on_motion_notify_event(GdkEventMotion* motion_event){
  if(motion_event->state != Gdk::BUTTON1_MASK)
    return true;

  
  if(!surface)
  {
    Cairo::RefPtr<Cairo::Context> c = get_window()->create_cairo_context();
    surface = c->get_target();
  }
    //surface = get_window()->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA, 100, 100);
    //surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,500,300);
    
  Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

  //cr->set_source(surface, 0, 0);
  cr->set_source_rgba(0, 1.0, 0, 1.0); 

  cr->rectangle(motion_event->x-5, motion_event->y-5, 10, 10);

  cr->fill();
  //surface->finish();
  
  return true;
}

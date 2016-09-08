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
  //surface = get_window()->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA, 1000, 1000);
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

  //auto cr2 = Cairo::Context::create(surface);
  //cr2->paint();
  
  return true;
}

bool MyArea::on_motion_notify_event(GdkEventMotion* motion_event){
  if(motion_event->state != Gdk::BUTTON1_MASK)
    return false;
#if 1
  //if(!surface)
    surface = get_window()->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA, 1000, 1000);

  auto cr = Cairo::Context::create(surface);
  cr->set_source_rgba(0, 255, 0, 0.6);
  cr->rectangle(motion_event->x, motion_event->y, 100, 100);

  cr->fill();

#endif
  return true;
}

//bool MyArea::on_button_press_event(GdkEventButton* release_event){
//std::cout << "press" << std::endl;
//return true;
//}

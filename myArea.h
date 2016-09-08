#ifndef GTKMM_EXAMPLE_MYAREA_H
#define GTKMM_EXAMPLE_MYAREA_H

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <climits>


class MyArea : public Gtk::DrawingArea
{
public:
  MyArea();
  Glib::RefPtr<Gdk::Pixbuf> image;

protected:
  //Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_motion_notify_event(GdkEventMotion* motion_event) override;
  Cairo::RefPtr<Cairo::Surface> surface;
};

#endif // GTKMM_EXAMPLE_MYAREA_H

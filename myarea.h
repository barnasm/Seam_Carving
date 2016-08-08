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
  void fitToWindow();
  virtual ~MyArea();
  std::vector<int> findVerticalSeam();

protected:
  //Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

  Glib::RefPtr<Gdk::Pixbuf> m_image_org, m_image_cpy, m_image_tmp, imageEnergy;
  std::shared_ptr<std::vector<std::vector<long long int>>> pixelsEnergy;

private:
  Glib::RefPtr<Gdk::Pixbuf> imageEnergyVisualization(Glib::RefPtr<Gdk::Pixbuf> image);
  int computePixelEnergy(const Glib::RefPtr<Gdk::Pixbuf> image, int x, int y);
};

#endif // GTKMM_EXAMPLE_MYAREA_H

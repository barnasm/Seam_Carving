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
#include <iostream>
#include "myarea.h"
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtk/gtk.h>

void on_my_check_resize(MyArea &myArea)
{
    myArea.fitToWindow();
}


int main(int argc, char** argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
     
    Gtk::Window win;
    win.set_title("DrawingArea");
    win.set_default_size(800, 600);
    
    
    MyArea area;
    win.add(area);
    win.signal_check_resize().connect(sigc::bind<MyArea&>(sigc::ptr_fun(&on_my_check_resize), area ));
    area.show();
    
    return app->run(win);
}

#pragma once

#include <gtkmm.h>
#include "myArea.h"
#include "imageByGtkmm.h"
#include "seamCarving.h"


class GtkmmGui : public Gtk::Window
{
private:
  void setMainWindow();
  void setSignals();
  
public:
  GtkmmGui();

protected:

  // Signal handlers:
  // Our new improved on_button_clicked(). (see below)
  void on_button_clicked_open_image(Glib::ustring);
  void on_my_check_resize(Glib::ustring);
  void on_combo_changed();
  // Child widgets:


  Gtk::Box m_box1, m_box2;
  Gtk::Button buttonOpenImage;

  Gtk::ComboBoxText comboBoxEnergy;
  
  MyArea areaImage;
  ImageByGtkmm *image;

  MyArea areaEnergy;
  ImageByGtkmm *imageEnergy;

  SeamCarving *seamCarving;
  std::shared_ptr<PixelEnergy> en;

  Gtk::Window *scWindow;
  MyArea scArea;
  Image *scImage;
};

#pragma once

#include <gtkmm.h>
#include <memory>
#include "myArea.h"
#include "imageByGtkmm.h"
#include "seamCarving.h"


class GtkmmGui : public Gtk::Window
{
private:
  void setMainWindow();
  void setSignals();
  void setShown();
  
public:
  GtkmmGui();

protected:

  void on_button_clicked_open_image();
  void on_button_clicked_protect_pixels(Glib::ustring);
  void on_button_clicked_remove_pixels(Glib::ustring);
  void on_my_check_resize(Glib::ustring);
  void on_combo_changed();

  //boxes
  Gtk::Box box1, box2;
  //buttons
  Gtk::Button buttonOpenImage, buttonRemovePixels, buttonProtectPixels;
  //combobox
  Gtk::ComboBoxText comboBoxEnergy;

  //areas
  MyArea areaSc;
  MyArea areaImage;
  MyArea areaEnergy;

  //images
  std::unique_ptr<ImageByGtkmm> imageOriginal;
  std::unique_ptr<ImageByGtkmm> imageEnergy;
  std::unique_ptr<Image> imageSc;

  std::unique_ptr<SeamCarving> seamCarving;
  std::shared_ptr<PixelEnergy> en;

  Gtk::Window windowSc;
};

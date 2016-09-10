#include "gtkmmGui.h"
#include <iostream>

GtkmmGui::GtkmmGui()
  :box1(Gtk::ORIENTATION_VERTICAL, 5),
   box2(Gtk::ORIENTATION_HORIZONTAL, 5),
   buttonOpenImage("Open image"),
   buttonRemovePixels("Remove"),
   buttonProtectPixels("Protect")
   //seamCarving(nullptr)
{
  setMainWindow();

  windowSc.set_keep_above(true);
  windowSc.add(areaSc);

  imageOriginal = std::unique_ptr<ImageByGtkmm>{new ImageByGtkmm(areaImage.image)};
  imageEnergy   = std::unique_ptr<ImageByGtkmm>(new ImageByGtkmm(areaEnergy.image));
  imageSc       = std::unique_ptr<Image>       {new ImageByGtkmm(areaSc.image)};

  setSignals();
  setShown();
  //show_all();
  //windowSc.show();
}

void GtkmmGui::setMainWindow(){
  set_title("Seam Carving");
  set_default_size(800, 600);
  set_border_width(10);
  
  box2.pack_start(buttonOpenImage,     Gtk::PACK_SHRINK);
  box2.pack_start(buttonRemovePixels,  Gtk::PACK_SHRINK);
  box2.pack_start(buttonProtectPixels, Gtk::PACK_SHRINK);
  box2.pack_start(comboBoxEnergy,      Gtk::PACK_SHRINK);

  {//Fill the combo:
    comboBoxEnergy.append("Simple");
    comboBoxEnergy.append("Simple vertical");
    comboBoxEnergy.append("Simple horizontal");
    comboBoxEnergy.append("Random");
    comboBoxEnergy.set_active(0);
  }

  box1.pack_start(box2, Gtk::PACK_SHRINK);
  box1.pack_start(areaImage);
  box1.pack_start(areaEnergy);
  add(box1);
}

void GtkmmGui::setShown(){
  buttonOpenImage.show();
  buttonProtectPixels.show();
  buttonRemovePixels.show();
  comboBoxEnergy.show();
  areaImage.show();
  areaEnergy.show();
  areaSc.show();
  box1.show();
  box2.show();
  show();
}

void GtkmmGui::setSignals(){
  areaImage.set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
  
  buttonOpenImage.signal_clicked().
    connect(sigc::mem_fun(*this, &GtkmmGui::on_button_clicked_open_image));
  
  comboBoxEnergy.signal_changed().connect(sigc::mem_fun(*this, &GtkmmGui::on_combo_changed));
#if 1
  signal_check_resize().
    connect(sigc::bind<Glib::ustring>(sigc::mem_fun
				      (*this, &GtkmmGui::on_my_check_resize),
				      "mainWindow"));

  windowSc.
    signal_check_resize().
    connect(sigc::bind<Glib::ustring>(sigc::mem_fun
				      (*this, &GtkmmGui::on_my_check_resize),
				      "scWindow"));
#endif
}


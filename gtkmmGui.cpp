#include "gtkmmGui.h"
#include <iostream>

void GtkmmGui::setMainWindow(){
  set_title("Seam Carving");
  set_default_size(800, 600);
  set_border_width(10);

  add(box1);
}

void GtkmmGui::setShown(){
  buttonOpenImage.show();
  buttonProtectPixels.show();
  buttonRemovePixels.show();
  comboBoxEnergy.show();
  //areaImage.show();
  //areaEnergy.show();
  //areaSc.show();
  box1.show();
  box2.show();
}

void GtkmmGui::setSignals(){
  buttonOpenImage.signal_clicked().
    connect(sigc::mem_fun(*this, &GtkmmGui::on_button_clicked_open_image));
  
  comboBoxEnergy.signal_changed().connect(sigc::mem_fun(*this, &GtkmmGui::on_combo_changed));
#if 0

  signal_check_resize().
    connect(sigc::bind<Glib::ustring>(sigc::mem_fun
				      (*this, &GtkmmGui::on_my_check_resize),
				      "mainWindow"));

  scWindow->
    signal_check_resize().
    connect(sigc::bind<Glib::ustring>(sigc::mem_fun
				      (*this, &GtkmmGui::on_my_check_resize),
				      "scWindow"));
#endif
}

GtkmmGui::GtkmmGui()
  :box1(Gtk::ORIENTATION_VERTICAL, 5),
   box2(Gtk::ORIENTATION_HORIZONTAL, 5),
   buttonOpenImage("Open image"),
   buttonRemovePixels("Remove"),
   buttonProtectPixels("Protect")
   //seamCarving(nullptr)
{
  setMainWindow();

  box2.pack_start(buttonOpenImage,     Gtk::PACK_SHRINK);
  box2.pack_start(buttonRemovePixels,  Gtk::PACK_SHRINK);
  box2.pack_start(buttonProtectPixels, Gtk::PACK_SHRINK);
  box2.pack_start(comboBoxEnergy,      Gtk::PACK_SHRINK);
  {
    //Fill the combo:
    comboBoxEnergy.append("Simple");
    comboBoxEnergy.append("Simple vertical");
    comboBoxEnergy.append("Simple horizontal");
    comboBoxEnergy.append("Random");
    comboBoxEnergy.set_active(0);
  }

  box1.pack_start(box2, Gtk::PACK_SHRINK);
  box1.pack_start(areaImage);
  //box1.pack_start(areaEnergy);

  areaImage.set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
  
  //windowSc = Gtk::Window();
  windowSc.set_keep_above(true);
  //windowSc.add(areaSc);

  imageOriginal = std::unique_ptr<ImageByGtkmm>{new ImageByGtkmm(areaImage.image)};
#if 0
  imageEnergy = new ImageByGtkmm(areaEnergy.image);
  imageSc = new ImageByGtkmm(scArea.image);
#endif

  show_all();
  setSignals();
}

void GtkmmGui::on_combo_changed()
{
#if 0
  if(seamCarving == nullptr)
    return;
  
  Glib::ustring text = comboBoxEnergy.get_active_text();
  if(!(text.empty())){
    switch(comboBoxEnergy.get_active_row_number()){
      case 0:
	en.reset();
      en = std::make_shared<EnergySimple>(EnergySimple::BOTH);
      break;
    case 1:
      en = std::make_shared<EnergySimple>(EnergySimple::VERTICAL);
      break;
    case 2:
      en = std::make_shared<EnergySimple>(EnergySimple::HORIZONTAL);
      break;
    case 3:
      en = std::make_shared<EnergyRandom>();
      break;
    default:
      std::cout << "wrong energy combobox row number" << std::endl;
      break;
    }
    
    seamCarving->setEnergyProvider(en);
    }
  else
    std::cout << "invalid energy combobox" << std::endl;
#endif
}

void GtkmmGui::on_my_check_resize(Glib::ustring data){
#if 0
  if(data == "scWindow"){
    seamCarving->sizeChanged(scArea.get_allocation().get_width(),
			   scArea.get_allocation().get_height());
  }
  //if(data == "mainWindow" || true)
  {
    image      ->fitToWindow(areaImage.get_allocation().get_width(),
			     areaImage.get_allocation().get_height());
    
    imageEnergy->fitToWindow(areaEnergy.get_allocation().get_width(),
			     areaEnergy.get_allocation().get_height());
  }
  areaEnergy.queue_draw();
#endif
}

void GtkmmGui::on_button_clicked_open_image()
{
   Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
   dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  dialog.set_current_folder("image/");

  //Add filters, so that only certain file types can be selected:

  auto filter_image = Gtk::FileFilter::create();
  filter_image->set_name("Image file");
  filter_image->add_pixbuf_formats();
  //filter_text->add_mime_type("text/plain");
  dialog.add_filter(filter_image);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  //Show the dialog and wait for a user response:
  //int result = dialog.run();

  //Handle the response:
  switch(dialog.run())
    {
    case(Gtk::RESPONSE_OK):
      {
	std::cout << "Open clicked." << std::endl;

	//Notice that this is a std::string, not a Glib::ustring.
	std::string filename = dialog.get_filename();
	std::cout << "File selected: " <<  filename << std::endl;
	imageOriginal->openFromFile(filename);
#if 0

	imageEnergy->setSameSize(*image);

	windowSc.hide();
	seamCarving = new SeamCarving(*image, *imageEnergy, *scImage, en);

	windowSc.resize_to_geometry(image->getWidth(), image->getHeight());
	windowSc.set_keep_above(true);
	windowSc.show();
	seamCarving->block = false;
	#endif
	break;
      }
    case(Gtk::RESPONSE_CANCEL):
      {
	std::cout << "Cancel clicked." << std::endl;
	break;
      }
    default:
      {
	std::cout << "Unexpected button clicked." << std::endl;
	break;
      }
    }
}

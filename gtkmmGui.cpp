#include "gtkmmGui.h"
#include <iostream>

void GtkmmGui::setMainWindow(){
  set_title("Seam Carving");
  set_default_size(800, 600);
  set_border_width(10);

  add(m_box1);
}
void GtkmmGui::setSignals(){
  buttonOpenImage.signal_clicked().
    connect(sigc::bind<Glib::ustring>(sigc::mem_fun(*this, &GtkmmGui::on_button_clicked_open_image),
				      "button 1"));
  
  comboBoxEnergy.signal_changed().connect(sigc::mem_fun(*this, &GtkmmGui::on_combo_changed) );

  signal_check_resize().connect(sigc::bind<Glib::ustring>(sigc::mem_fun
							  (*this, &GtkmmGui::on_my_check_resize),
							  "mainWindow"));

  scWindow->signal_check_resize().connect(sigc::bind<Glib::ustring>(sigc::mem_fun
								    (*this, &GtkmmGui::on_my_check_resize),
								    "scWindow"));
}

GtkmmGui::GtkmmGui()
  :m_box1(Gtk::ORIENTATION_VERTICAL, 5),
   m_box2(Gtk::ORIENTATION_HORIZONTAL, 5),
   buttonOpenImage("Open image"),
   seamCarving(nullptr)
{
  setMainWindow();

  m_box2.pack_start(buttonOpenImage, Gtk::PACK_SHRINK);
  {
    //Fill the combo:
    comboBoxEnergy.append("Simple");
    comboBoxEnergy.append("Simple vertical");
    comboBoxEnergy.append("Simple horizontal");
    comboBoxEnergy.append("Random");
    comboBoxEnergy.set_active(0);
  }

  m_box2.pack_start(comboBoxEnergy, Gtk::PACK_SHRINK);
  comboBoxEnergy.show();

  m_box1.pack_start(m_box2, Gtk::PACK_SHRINK);
  buttonOpenImage.show();

  m_box1.pack_start(areaImage);
  m_box1.pack_start(areaEnergy);

  areaImage.set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
  //areaImage.add_events(Gdk::BUTTON_PRESS_MASK);
    
  areaImage.show();
  areaEnergy.show();

  scWindow = new Gtk::Window();
  scWindow->set_keep_above(true);
  scWindow->add(scArea);
  scArea.show();
  
  image  = new ImageByGtkmm(areaImage.image);
  imageEnergy = new ImageByGtkmm(areaEnergy.image);
  scImage = new ImageByGtkmm(scArea.image);

  m_box1.show();
  m_box2.show();

  setSignals();
}

void GtkmmGui::on_combo_changed()
{
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

}

void GtkmmGui::on_my_check_resize(Glib::ustring data){
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
}

void GtkmmGui::on_button_clicked_open_image(Glib::ustring data)
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
	image->openFromFile(filename);
	imageEnergy->setSameSize(*image);

	scWindow->hide();
	seamCarving = new SeamCarving(*image, *imageEnergy, *scImage, en);

	scWindow->resize_to_geometry(image->getWidth(), image->getHeight());
	scWindow->set_keep_above(true);
	scWindow->show();
	seamCarving->block = false;
 
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

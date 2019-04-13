#include "gtkmmGui.h"
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
    seamCarving->sizeChanged(areaSc.get_allocation().get_width(),
    			     areaSc.get_allocation().get_height());
  }
  //if(data == "mainWindow" || true)
  {
    imageOriginal->fitToWindow(areaImage.get_allocation().get_width(),
			     areaImage.get_allocation().get_height());
    
    imageEnergy->fitToWindow(areaEnergy.get_allocation().get_width(),
			     areaEnergy.get_allocation().get_height());
  }
  areaEnergy.queue_draw();
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

	imageEnergy->setSameSize(imageOriginal->getWidth(), imageOriginal->getHeight());

	windowSc.hide();
	seamCarving =
	  std::unique_ptr<SeamCarving>{new SeamCarving(*imageOriginal, *imageEnergy, *imageSc, en)};

	windowSc.resize_to_geometry(imageOriginal->getWidth(), imageOriginal->getHeight());
	windowSc.show();
	windowSc.set_keep_above(true);
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

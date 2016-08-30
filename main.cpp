#include <iostream>
#include "gtkmmGui.h"
#include "seamCarving.h"

void test(){
  std::cout << "***TEST***" << std::endl;
  SeamCarving sc;
  //sc.test(10, 10);
}


int main(int argc, char** argv)
{
  //#define TEST
#ifdef TEST
  test();
  return 0;
#else
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
  GtkmmGui win;
  return app->run(win);
#endif
}

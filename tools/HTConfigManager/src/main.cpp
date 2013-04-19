/**
 */

#include "CMainWindow.h"

#include <QApplication>
#include <QDebug>



///////////////////////
//// Main Program
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  CMainWindow main_window;


  main_window.show();
  //main_window.showMaximized();

  return app.exec();
}

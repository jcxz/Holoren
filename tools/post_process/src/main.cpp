/**
 */

#include "CMainWindow.h"

#include <QApplication>
#include <QDesktopWidget>



///////////////////////
//// Main Program
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  CMainWindow main_window;

  QDesktopWidget desktop;

  main_window.showMaximized();

  return app.exec();
}

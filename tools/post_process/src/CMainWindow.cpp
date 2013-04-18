/**
 */

#include "CMainWindow.h"
#include "CConfigViewer.h"
#include "CDFtoHologramEditor.h"

#include <QMessageBox>



/**
 */
CMainWindow::CMainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_cfg_viewer(0)
{
  m_cfg_viewer = new CConfigViewer("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df");

  addDockWidget(Qt::LeftDockWidgetArea, m_cfg_viewer);

  CDFtoHologramEditor *e = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\DFtoHologram.xml");
  //CDFtoHologramEditor *e = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\test.xml");
  e->setDFtoHologramPath("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\holo_toolkit\\DFtoHologram.exe");
  e->setDFtoHologramWorkingDir("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\");
  connect(e, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  connect(e, SIGNAL(utilityFinished()), SLOT(handleUtilityFinished()));

  setCentralWidget(e);
}


/**
 */
void CMainWindow::handleError(const QString & err)
{
  QMessageBox::critical(this, "Config Manager", err);
  return;
}


/**
 */
void CMainWindow::handleUtilityFinished(void)
{
  QMessageBox::information(this, "Config Manager", "Utility successfully finished");
  return;
}

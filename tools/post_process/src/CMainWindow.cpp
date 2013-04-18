/**
 */

#include "CMainWindow.h"
#include "CConfigViewer.h"
#include "CDFtoHologramEditor.h"
#include "CHoloPropagLargeEditor.h"

#include <QHBoxLayout>
#include <QMessageBox>



/**
 */
CMainWindow::CMainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_cfg_viewer(0)
{
  //m_cfg_viewer = new CConfigViewer("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df");

  //addDockWidget(Qt::LeftDockWidgetArea, m_cfg_viewer);

  //CDFtoHologramEditor *e = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\test.xml");

  /* DFtoHologram Editor */
  CDFtoHologramEditor *df_editor = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\DFtoHologram.xml");
  df_editor->setDFtoHologramPath("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\holo_toolkit\\DFtoHologram.exe");
  df_editor->setDFtoHologramWorkingDir("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\");
  connect(df_editor, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  connect(df_editor, SIGNAL(utilityFinished()), SLOT(handleUtilityFinished()));

  /* HoloPropagLarge Editor */
  CHoloPropagLargeEditor *propag_editor = new CHoloPropagLargeEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\HoloPropagLarge.xml");
  propag_editor->setHoloPropagLargePath("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\holo_toolkit\\HoloPropagLarge.exe");
  propag_editor->setHoloPropagLargeWorkingDir("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\");
  connect(propag_editor, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  connect(propag_editor, SIGNAL(utilityFinished()), SLOT(handleUtilityFinished()));

  QHBoxLayout *l = new QHBoxLayout;
  l->addWidget(df_editor);
  l->addWidget(propag_editor);

  QWidget *container = new QWidget;
  container->setLayout(l);

  setCentralWidget(container);
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

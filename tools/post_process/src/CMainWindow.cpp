/**
 */

#include "CMainWindow.h"
//#include "CConfigViewer.h"
#include "CDFtoHologramEditor.h"
#include "CHoloPropagLargeEditor.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QShowEvent>
#include <QDesktopWidget>
#include <QApplication>



/**
 */
CMainWindow::CMainWindow(QWidget *parent)
  : QMainWindow(parent),
    //m_cfg_viewer(0)
    m_df_editor(0),
    m_propag_editor(0)
{
  //m_cfg_viewer = new CConfigViewer("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df");
  //addDockWidget(Qt::LeftDockWidgetArea, m_cfg_viewer);

  //CDFtoHologramEditor *e = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\test.xml");

  /* DFtoHologram Editor */
  //m_df_editor = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\DFtoHologram.xml");
  //m_df_editor->setDFtoHologramPath("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\holo_toolkit\\DFtoHologram.exe");
  //m_df_editor->setDFtoHologramWorkingDir("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\");

  m_df_editor = new CDFtoHologramEditor;
  connect(m_df_editor, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  connect(m_df_editor, SIGNAL(utilityFinished()), SLOT(handleDFtoHologramFinished()));
  m_df_editor->loadHistory("./DFtoHologramEditorHistory.xml");

  /* HoloPropagLarge Editor */
  //m_propag_editor = new CHoloPropagLargeEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\HoloPropagLarge.xml");
  //m_propag_editor->setHoloPropagLargePath("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\holo_toolkit\\HoloPropagLarge.exe");
  //m_propag_editor->setHoloPropagLargeWorkingDir("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\");

  m_propag_editor = new CHoloPropagLargeEditor;
  connect(m_propag_editor, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  connect(m_propag_editor, SIGNAL(utilityFinished()), SLOT(handleHoloPropagLargeFinished()));
  m_propag_editor->loadHistory("./HoloPropagLargeHistory.xml");

  /* set layout */
  QHBoxLayout *l = new QHBoxLayout;
  l->addWidget(m_df_editor);
  l->addWidget(m_propag_editor);

  QWidget *container = new QWidget;
  container->setLayout(l);

  setCentralWidget(container);
}


/**
 */
CMainWindow::~CMainWindow(void)
{
  m_df_editor->saveHistory("./DFtoHologramEditorHistory.xml");
  m_propag_editor->saveHistory("./HoloPropagLargeHistory.xml");
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
void CMainWindow::handleDFtoHologramFinished(void)
{
  QMessageBox::information(this, "Config Manager", "DFtoHologram utility successfully finished");
  return;
}


/**
 */
void CMainWindow::handleHoloPropagLargeFinished(void)
{
  QMessageBox::information(this, "Config Manager", "HoloPropagLarge utility successfully finished");
  return;
}


/**
 */
void CMainWindow::showEvent(QShowEvent *event)
{
  int w = width();
  int h = height();
  QDesktopWidget *desktop = qApp->desktop();

  // available geometry is the screen dimensions without taskbars and the like
  QRect desktop_rect = desktop->availableGeometry();

  setGeometry((desktop_rect.width() - w) / 2.0f, (desktop_rect.height() - h) / 2.0f, w, h);

  return QWidget::showEvent(event);
}

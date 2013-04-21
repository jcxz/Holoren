/**
 */

#include "CMainWindow.h"
//#include "CConfigViewer.h"
#include "CDFtoHologramEditor.h"
#include "CHoloPropagLargeEditor.h"
#include "CConsoleWidget.h"
#include "CSettingsDlg.h"

#include <QMenuBar>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QShowEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>



/**
 */
CMainWindow::CMainWindow(QWidget *parent)
  : QMainWindow(parent),
    //m_cfg_viewer(0)
    m_df_editor(0),
    m_propag_editor(0),
    m_settings_dlg(0),
    m_settings_act(0),
    m_exit_act(0),
    m_console_act(0)
{
  //m_cfg_viewer = new CConfigViewer("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df");
  //addDockWidget(Qt::LeftDockWidgetArea, m_cfg_viewer);

  //CDFtoHologramEditor *e = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\test.xml");

  /* DFtoHologram Editor */
  //m_df_editor = new CDFtoHologramEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\DFtoHologram.xml");
  //m_df_editor->setDFtoHologramPath("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\holo_toolkit\\DFtoHologram.exe");
  //m_df_editor->setDFtoHologramWorkingDir("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\");

  m_settings_dlg = new CSettingsDlg("./Settings.xml");
  connect(m_settings_dlg, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  //connect(m_settings_dlg, SIGNAL(saved()), SLOT(handleSettingsSaved()));

  m_df_editor = new CDFtoHologramEditor("./DFtoHologramEditorHistory.xml");
  connect(m_df_editor, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  connect(m_df_editor, SIGNAL(utilityFinished()), SLOT(handleDFtoHologramFinished()));
  //m_df_editor->loadHistory("./DFtoHologramEditorHistory.xml");

  /* HoloPropagLarge Editor */
  //m_propag_editor = new CHoloPropagLargeEditor("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\HoloPropagLarge.xml");
  //m_propag_editor->setHoloPropagLargePath("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\holo_toolkit\\HoloPropagLarge.exe");
  //m_propag_editor->setHoloPropagLargeWorkingDir("D:\\AC601\\VirtualBOXShare\\Ubuntu\\IBP\\code\\v4\\df\\");

  m_propag_editor = new CHoloPropagLargeEditor("./HoloPropagLargeEditorHistory.xml");
  connect(m_propag_editor, SIGNAL(error(QString)), SLOT(handleError(const QString &)));
  connect(m_propag_editor, SIGNAL(utilityFinished()), SLOT(handleHoloPropagLargeFinished()));
  //m_propag_editor->loadHistory("./HoloPropagLargeHistory.xml");

  /* set layout */
  QHBoxLayout *l = new QHBoxLayout;
  l->addWidget(m_df_editor);
  l->addWidget(m_propag_editor);

  QWidget *container = new QWidget;
  container->setLayout(l);

  setCentralWidget(container);

  /* create menu-s and menu actions */
  createActions();
  createMenus();
}


/**
 */
void CMainWindow::createActions(void)
{
  m_settings_act = new QAction(tr("&Settings"), this);
  connect(m_settings_act, SIGNAL(triggered()), m_settings_dlg, SLOT(exec()));

  m_exit_act = new QAction(tr("&Exit"), this);
  connect(m_exit_act, SIGNAL(triggered()), SLOT(close()));

  m_console_act = new QAction(tr("&Console Window"), this);
  connect(m_console_act, SIGNAL(triggered()), SLOT(showConsole()));

  return;
}


/**
 */
void CMainWindow::createMenus(void)
{
  QMenuBar *menu_bar = menuBar();

  /* File menu */
  QMenu *file_menu = menu_bar->addMenu("&File");
  file_menu->addAction(m_settings_act);
  file_menu->addSeparator();
  file_menu->addAction(m_exit_act);

  /* View Menu menu */
  QMenu *view_menu = menu_bar->addMenu("&View");
  view_menu->addAction(m_console_act);

  return;
}


/**
 */
CMainWindow::~CMainWindow(void)
{
  //if (!m_settings_dlg->save("./settings.xml"))
  //{
  //  qDebug() << "Failed to save settings";
  //}

  if (!m_df_editor->saveHistory("./DFtoHologramEditorHistory.xml"))
  {
    qDebug() << "Failed to save DFtoHologramEditor history file";
  }

  if (!m_propag_editor->saveHistory("./HoloPropagLargeEditorHistory.xml"))
  {
    qDebug() << "Failed to save HoloPropagLargeEditor history file";
  }
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
void CMainWindow::handleSettingsSaved(void)
{
  QMessageBox::information(this, "Config Manager", "Settings were successfully saved");
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
void CMainWindow::handleConsoleFinished(void)
{
  // last console command history overwrites the histories of any preceding
  // console sessions
  if (!static_cast<CConsoleWidget *>(sender())->saveHistory("./ConsoleHistory.xml"))
  {
    qDebug() << "Failed to save the last console command history";
  }

  return;
}


/**
 */
void CMainWindow::showConsole(void)
{
  CConsoleWidget *console = new CConsoleWidget(m_settings_dlg->getStartScript(), "./ConsoleHistory.xml");
  connect(console, SIGNAL(finished()), SLOT(handleConsoleFinished()));
  connect(console, SIGNAL(error(QString)), SLOT(handleError(QString)));
  connect(console, SIGNAL(destroyed()), SLOT(deleteLater()));   /// destroy the console once it is closed
  console->resize(700, 400);
  console->show();
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

/**
 * The code in this file has been adopted from Qt ImageViewer demo
 * @see http://qt-project.org/doc/qt-4.8/widgets-imageviewer.html
 */

#include "CImageViewer.h"

#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QScrollArea>
#include <QScrollBar>
#include <QPrintDialog>
#include <QPainter>
#include <QMenuBar>



/**
 */
CImageViewer::CImageViewer(QWidget *parent)
  : QMainWindow(parent),
    m_image_label(0),
    m_scroll_area(0),
    m_scale_factor(0.0f),

#ifndef QT_NO_PRINTER
    m_printer(),
#endif

    m_print_act(0),
    m_exit_act(0),
    m_zoom_in_act(0),
    m_zoom_out_act(0),
    m_normal_size_act(0),
    m_fit_to_window_act(0),
    m_file_menu(0),
    m_view_menu(0)
{
  m_image_label = new QLabel;
  m_image_label->setBackgroundRole(QPalette::Base);
  m_image_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  m_image_label->setScaledContents(true);

  m_scroll_area = new QScrollArea;
  m_scroll_area->setBackgroundRole(QPalette::Dark);
  m_scroll_area->setWidget(m_image_label);

  setCentralWidget(m_scroll_area);

  createActions();
  createMenus();

  resize(500, 400);
}


/**
 */
bool CImageViewer::open(const QString & path)
{
  QImage image(path);
  if (image.isNull())
  {
    return false;
  }

  m_image_label->setPixmap(QPixmap::fromImage(image));
  m_scale_factor = 1.0;

  m_print_act->setEnabled(true);
  m_fit_to_window_act->setEnabled(true);
  updateActions();

  if (!m_fit_to_window_act->isChecked())
  {
    m_image_label->adjustSize();
  }

  return true;
}


/**
 */
void CImageViewer::print(void)
{
  Q_ASSERT(m_image_label->pixmap());

#ifndef QT_NO_PRINTER
  QPrintDialog dialog(&m_printer, this);
  if (dialog.exec())
  {
    QPainter painter(&m_printer);
    QRect rect = painter.viewport();
    QSize size = m_image_label->pixmap()->size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow(m_image_label->pixmap()->rect());
    painter.drawPixmap(0, 0, *m_image_label->pixmap());
  }
#endif

  return;
}


/**
 */
void CImageViewer::zoomIn(void)
{
  scaleImage(1.25);
  return;
}


/**
 */
void CImageViewer::zoomOut(void)
{
  scaleImage(0.8);
  return;
}


/**
 */
void CImageViewer::normalSize(void)
{
  m_image_label->adjustSize();
  m_scale_factor = 1.0;
  return;
}


/**
 */
void CImageViewer::fitToWindow(void)
{
  bool fit_to_window = m_fit_to_window_act->isChecked();
  m_scroll_area->setWidgetResizable(fit_to_window);
  if (!fit_to_window)
  {
    normalSize();
  }

  updateActions();

  return;
}


/**
 */
void CImageViewer::scaleToImage(void)
{
  m_image_label->adjustSize();
  m_scale_factor = 1.0;
  resize(m_image_label->pixmap()->size());
  m_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  return;
}


/**
 */
void CImageViewer::createActions(void)
{
  m_print_act = new QAction(tr("&Print..."), this);
  m_print_act->setShortcut(tr("Ctrl+P"));
  m_print_act->setEnabled(false);
  connect(m_print_act, SIGNAL(triggered()), SLOT(print()));

  m_exit_act = new QAction(tr("E&xit"), this);
  m_exit_act->setShortcut(tr("Ctrl+Q"));
  connect(m_exit_act, SIGNAL(triggered()), SLOT(close()));

  m_zoom_in_act = new QAction(tr("Zoom &In (25%)"), this);
  m_zoom_in_act->setShortcut(tr("Ctrl++"));
  m_zoom_in_act->setEnabled(false);
  connect(m_zoom_in_act, SIGNAL(triggered()), SLOT(zoomIn()));

  m_zoom_out_act = new QAction(tr("Zoom &Out (25%)"), this);
  m_zoom_out_act->setShortcut(tr("Ctrl+-"));
  m_zoom_out_act->setEnabled(false);
  connect(m_zoom_out_act, SIGNAL(triggered()), SLOT(zoomOut()));

  m_normal_size_act = new QAction(tr("&Normal Size"), this);
  m_normal_size_act->setShortcut(tr("Ctrl+S"));
  m_normal_size_act->setEnabled(false);
  connect(m_normal_size_act, SIGNAL(triggered()), SLOT(normalSize()));

  m_fit_to_window_act = new QAction(tr("&Fit to Window"), this);
  m_fit_to_window_act->setEnabled(false);
  m_fit_to_window_act->setCheckable(true);
  m_fit_to_window_act->setShortcut(tr("Ctrl+F"));
  connect(m_fit_to_window_act, SIGNAL(triggered()), SLOT(fitToWindow()));

  m_scale_to_image_act = new QAction(tr("&Scale to image size"), this);
  m_scale_to_image_act->setCheckable(true);
  m_scale_to_image_act->setShortcut(tr("Ctrl+I"));
  connect(m_scale_to_image_act, SIGNAL(triggered()), SLOT(scaleToImage()));

  return;
}


/**
 */
void CImageViewer::createMenus(void)
{
  m_file_menu = new QMenu(tr("&File"), this);
  m_file_menu->addAction(m_print_act);
  m_file_menu->addSeparator();
  m_file_menu->addAction(m_exit_act);

  m_view_menu = new QMenu(tr("&View"), this);
  m_view_menu->addAction(m_zoom_in_act);
  m_view_menu->addAction(m_zoom_out_act);
  m_view_menu->addAction(m_normal_size_act);
  m_view_menu->addSeparator();
  m_view_menu->addAction(m_fit_to_window_act);
  m_view_menu->addAction(m_scale_to_image_act);

  menuBar()->addMenu(m_file_menu);
  menuBar()->addMenu(m_view_menu);

  return;
}


/**
 */
void CImageViewer::updateActions(void)
{
  m_zoom_in_act->setEnabled(!m_fit_to_window_act->isChecked());
  m_zoom_out_act->setEnabled(!m_fit_to_window_act->isChecked());
  m_normal_size_act->setEnabled(!m_fit_to_window_act->isChecked());

  return;
}


/**
 */
void CImageViewer::scaleImage(double factor)
{
  Q_ASSERT(m_image_label->pixmap());

  m_scale_factor *= factor;
  m_image_label->resize(m_scale_factor * m_image_label->pixmap()->size());

  adjustScrollBar(m_scroll_area->horizontalScrollBar(), factor);
  adjustScrollBar(m_scroll_area->verticalScrollBar(), factor);

  m_zoom_in_act->setEnabled(m_scale_factor < 3.0);
  m_zoom_out_act->setEnabled(m_scale_factor > 0.333);

  return;
}


/**
 */
void CImageViewer::adjustScrollBar(QScrollBar *scroll_bar, double factor)
{
  scroll_bar->setValue(int(factor * scroll_bar->value() + ((factor - 1) * scroll_bar->pageStep() / 2)));
  return;
}

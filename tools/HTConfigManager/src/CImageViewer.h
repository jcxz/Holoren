/**
 * The code in this file has been adopted from Qt ImageViewer demo
 * @see http://qt-project.org/doc/qt-4.8/widgets-imageviewer.html
 */

#ifndef CIMAGEVIEWER_H
#define CIMAGEVIEWER_H

#include <QMainWindow>
#include <QPrinter>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;


/**
 * A class to display a dialog with image
 */
class CImageViewer : public QMainWindow
{
    Q_OBJECT

  public:
    CImageViewer(QWidget *parent = 0);

    bool open(const QString & path);

  private slots:
    void print();
    void zoomIn(void);
    void zoomOut(void);
    void normalSize(void);
    void fitToWindow(void);

  private:
    void createActions(void);
    void createMenus(void);
    void updateActions(void);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scroll_bar, double factor);

  private:
    QLabel *m_image_label;
    QScrollArea *m_scroll_area;
    double m_scale_factor;

#ifndef QT_NO_PRINTER
    QPrinter m_printer;
#endif

    QAction *m_print_act;
    QAction *m_exit_act;

    QAction *m_zoom_in_act;
    QAction *m_zoom_out_act;
    QAction *m_normal_size_act;
    QAction *m_fit_to_window_act;

    QMenu *m_file_menu;
    QMenu *m_view_menu;
};

#endif // CIMAGEVIEWER_H

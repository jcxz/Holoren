/*
 * Copyright (C) 2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of Holoren.
 *
 * Holoren is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Holoren is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Holoren.  If not, see <http://www.gnu.org/licenses/>.
 */

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

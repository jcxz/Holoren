/**
 */

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>


class CConfigViewer;

/**
 */
class CMainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    /**
     * Default constructor
     */
    explicit CMainWindow(QWidget *parent = 0);

  private slots:
    void handleError(const QString & err);
    void handleUtilityFinished(void);

  private:
    CConfigViewer *m_cfg_viewer;
};

#endif

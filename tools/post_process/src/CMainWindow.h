/**
 */

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>


//class CConfigViewer;
class CDFtoHologramEditor;
class CHoloPropagLargeEditor;

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

    /**
     * Destructor
     */
    ~CMainWindow(void);

  private slots:
    void handleError(const QString & err);
    void handleDFtoHologramFinished(void);
    void handleHoloPropagLargeFinished(void);

  protected:
    virtual void showEvent(QShowEvent *event);

  private:
    //CConfigViewer *m_cfg_viewer;
    CDFtoHologramEditor *m_df_editor;
    CHoloPropagLargeEditor *m_propag_editor;
};

#endif

/**
 */

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>


//class CConfigViewer;
class CDFtoHologramEditor;
class CHoloPropagLargeEditor;
class CSettingsDlg;

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
    void handleSettingsSaved(void);
    void handleError(const QString & err);
    void handleDFtoHologramFinished(void);
    void handleHoloPropagLargeFinished(void);
    void handleConsoleFinished(void);

    /**
     * A slot to show console window
     */
    void showConsole(void);

  private:
    void createActions(void);
    void createMenus(void);

  protected:
    virtual void showEvent(QShowEvent *event);

  private:
    //CConfigViewer *m_cfg_viewer;
    CDFtoHologramEditor *m_df_editor;
    CHoloPropagLargeEditor *m_propag_editor;
    CSettingsDlg *m_settings_dlg;

    /* Menu Actions */
    QAction *m_settings_act; /// a menu action to manage application settings
    QAction *m_exit_act;     /// a menu action to close the program
    QAction *m_console_act;  /// a menu action to open console window
};

#endif

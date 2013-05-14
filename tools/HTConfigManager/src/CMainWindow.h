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

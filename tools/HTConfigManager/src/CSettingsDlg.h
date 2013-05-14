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

#ifndef CSETTINGSDLG_H
#define CSETTINGSDLG_H

#include <QDialog>
#include <QTextEdit>
#include <QString>


class QDialogButtonBox;

/**
 * A dialog with settings
 */
class CSettingsDlg : public QDialog
{
    Q_OBJECT

  public:
    /**
     * Default constructor
     */
    explicit CSettingsDlg(const QString & settings_file = QString(), QWidget *parent = 0);

    QString getStartScript(void) const
    {
      return m_start_script->toPlainText();
    }

    bool load(const QString & filename);
    bool save(const QString & filename);

  signals:
    void error(QString error);
    void saved(void);

  private slots:
    void handleAccept(void);

  private:
    QString m_settings_file;      /// a file with settings
    QTextEdit *m_start_script;    /// a text box with console start script
    QDialogButtonBox *m_buttons;  /// ok/cancel buttons
};

#endif // CSETTINGSDLG_H

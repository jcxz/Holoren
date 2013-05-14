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

#ifndef CCONFIGVIEWER_H
#define CCONFIGVIEWER_H

#include <QDockWidget>
#include <QString>


class QListWidget;

/**
 */
class CConfigViewer : public QDockWidget
{
    Q_OBJECT

  public:
    /**
     * Default constructor
     */
    explicit CConfigViewer(const QString & cfg_dir = QString(), QWidget *parent = 0);

    /**
     * A method to get the currently selected configuration directory
     */
    QString getSelectedConfigDir(void) const;

    /**
     * A method to set the configuration directory
     */
    void setConfigDir(const QString & cfg_dir)
    {
      m_cfg_dir = cfg_dir;
      return;
    }

    /**
     * A method to refresh the list of configuration files
     */
    void refresh(void);

  private:
    QListWidget *m_cfg_list;  /// a list of config files
    QString m_cfg_dir;        /// a directory with configuration files
};

#endif

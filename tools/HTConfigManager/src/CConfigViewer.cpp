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

#include "CConfigViewer.h"

#include <QListWidget>
#include <QDirIterator>
#include <QDebug>



/**
 */
CConfigViewer::CConfigViewer(const QString & cfg_dir, QWidget *parent)
  : QDockWidget(parent),
    m_cfg_list(0),
    m_cfg_dir(cfg_dir)
{
  m_cfg_list = new QListWidget;

  m_cfg_list->setSelectionMode(QAbstractItemView::SingleSelection);

  refresh();

  setWidget(m_cfg_list);
}


/**
 */
QString CConfigViewer::getSelectedConfigDir(void) const
{
  QListWidgetItem *item = m_cfg_list->currentItem();
  if (item == NULL)
  {
    return QString();
  }

  return item->data(Qt::UserRole).toString();
}


/**
 */
void CConfigViewer::refresh(void)
{
  m_cfg_list->clear();

  if (m_cfg_dir.isEmpty())
  {
    return;
  }

  QDirIterator cfg_dir_it(m_cfg_dir, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

  while (cfg_dir_it.hasNext())
  {
    cfg_dir_it.next();   // this has to be called first (for some weird reason)

    QListWidgetItem *item = new QListWidgetItem;

    item->setData(Qt::UserRole, cfg_dir_it.filePath());
    item->setText(cfg_dir_it.fileName());

    qDebug() << "full path:" << cfg_dir_it.filePath();
    qDebug() << "file name:" << cfg_dir_it.fileName();

    m_cfg_list->addItem(item);
  }

  return;
}

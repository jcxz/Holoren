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

#include "CPathPicker.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QShortcut>
#include <QDebug>



/**
 */
CPathPicker::CPathPicker(EMode mode, QWidget *parent)
  : QWidget(parent),
    m_mode(mode),
    m_paths(0),
    m_pb_pick(0),
    m_del_shortcut(0)
{
  m_paths = new QComboBox;
  m_paths->setEditable(true);
  m_paths->setDuplicatesEnabled(false);   // do not allow user to enter duplicate values (programatically inserting duplicates is still possible)
  connect(m_paths, SIGNAL(currentIndexChanged(int)), SIGNAL(picked()));
  //connect(m_paths, SIGNAL(highlighted(int)), SLOT(removePath(int)));

  /* make shortcut to delete items from combobox */
  m_del_shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this, SLOT(removePath()));
  m_del_shortcut->setEnabled(true);

  m_pb_pick = new QPushButton("...");
  m_pb_pick->setFixedSize(20, 20);
  connect(m_pb_pick, SIGNAL(clicked()), SLOT(handlePickBtnClicked()));

  QHBoxLayout *l = new QHBoxLayout;
  l->setContentsMargins(0, 0, 0, 0);  // get rid of unwanted margins on layout
  l->addWidget(m_paths);
  l->addWidget(m_pb_pick);

  setLayout(l);
  setContentsMargins(0, 0, 0, 0);   // get rid of unwanted margins on the widget itself
}


/**
 */
void CPathPicker::addPath(const QString & path)
{
#if defined(Q_OS_WIN) || defined(Q_OS_WIN32)
  int index = m_paths->findText(path);
#else
  int index = m_paths->findText(path);
#endif
  if (index != -1)
  { // just set the current index if found
    m_paths->setCurrentIndex(index);
  }
  else
  { // otherwise add it
    m_paths->addItem(path);
    m_paths->setCurrentIndex(m_paths->count() - 1);
  }

  return;
}


/**
 */
void CPathPicker::removePath(void)
{
  qDebug() << "deleting path:";
  m_paths->removeItem(m_paths->currentIndex());
  return;
}


/**
 */
void CPathPicker::handlePickBtnClicked(void)
{
  QFileInfo info(m_paths->currentText());

  if (m_mode == PICK_DIRS)
  {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Pick a directory"), info.absolutePath());
    if (!dir.isNull())
    {
      addPath(dir);
    }
  }
  else
  {
    QString file = QFileDialog::getOpenFileName(this, tr("Pick a file"), info.absolutePath());
    if (!file.isNull())
    {
      addPath(file);
    }
  }

  return;
}

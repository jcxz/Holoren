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

#ifndef CPATHPICKER_H
#define CPATHPICKER_H

#include <QWidget>
#include <QComboBox>


class QPushButton;
class QShortcut;

/**
 */
class CPathPickerIterator
{
  public:
    /**
     */
    CPathPickerIterator(QComboBox *paths)
      : m_cur_path(0),
        m_paths(paths)
    {
    }

    /**
     */
    inline bool hasNext(void) const
    {
      return m_cur_path < m_paths->count();
    }

    /**
     */
    inline QString getPath(void) const
    {
      return m_paths->itemText(m_cur_path);
    }

    /**
     */
    inline void next(void)
    {
      m_cur_path++;
      return;
    }

  private:
    int m_cur_path;
    QComboBox *m_paths;
};


/**
 * A class with widgets to pick a directory or a file
 *
 * TODO: make this class serializable (e.g by creating an ISerializable
*        interface a letting it derive from it ) instead of
 *       providing methods to get various internal data
 */
class CPathPicker : public QWidget
{
    Q_OBJECT

  public:
    /** Path picker mode */
    enum EMode {
      PICK_DIRS,   /// to pick directories
      PICK_FILES   /// to pick files
    };

  public:
    /**
     * Default constructor
     */
    explicit CPathPicker(EMode mode = PICK_FILES, QWidget *parent = 0);

    /**
     */
    void setMode(EMode mode)
    {
      m_mode = mode;
      return;
    }

    /**
     * A method to retrieve the selected path
     */
    QString getSelectedPath(void) const
    {
      return m_paths->currentText();
    }

    /**
     * A method to get the index of the selected path
     */
    int getSelectedPathIndex(void) const
    {
      return m_paths->currentIndex();
    }

    /**
     * A method to set the currently selected path
     */
    void setSelectedPath(int path_index)
    {
      m_paths->setCurrentIndex(path_index);
      return;
    }

    /**
     */
    void clearAllPaths(void)
    {
      m_paths->clear();
      return;
    }

    /**
     * A method to get a list of all items
     */
    CPathPickerIterator getPathsIterator(void) const
    {
      return CPathPickerIterator(m_paths);
    }

    /**
     * A method to add a path to the list of paths
     * with checking on duplicates.
     */
    void addPath(const QString & path);

  signals:
    /**
     * This signals is emitted when a user picks a new path
     */
    void picked(void);

  private slots:
    /**
     */
    void handlePickBtnClicked(void);

    /**
     * A slot to remove a given path from a list of paths
     */
    void removePath(void);

  private:
    EMode m_mode;             /// picker mode
    QComboBox *m_paths;       /// an edit box with the picked path
    QPushButton *m_pb_pick;   /// a button to change the selection
    QShortcut *m_del_shortcut; /// a shortcut to delete the item from combobox
};

#endif // CPATHPICKER_H

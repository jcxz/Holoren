/**
 */

#include "CPathPicker.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>



/**
 */
CPathPicker::CPathPicker(EMode mode, QWidget *parent)
  : QWidget(parent),
    m_mode(mode),
    m_paths(0),
    m_pb_pick(0)
{
  m_paths = new QComboBox;
  m_paths->setEditable(true);
  m_paths->setDuplicatesEnabled(false);   // do not allow user to enter duplicate values (programatically inserting duplicates is still possible)
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
  int index = m_paths->findText(path, Qt::CaseSensitive);
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
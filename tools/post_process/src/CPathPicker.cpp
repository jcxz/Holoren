/**
 */

#include "CPathPicker.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>



/**
 */
CPathPicker::CPathPicker(EMode mode, const QString & path, QWidget *parent)
  : QWidget(parent),
    m_mode(mode),
    m_path(0),
    m_pb_pick(0)
{
  m_path = new QLineEdit(path);
  m_pb_pick = new QPushButton("...");
  m_pb_pick->setFixedSize(20, 20);
  connect(m_pb_pick, SIGNAL(clicked()), SLOT(handlePickBtnClicked()));

  QHBoxLayout *l = new QHBoxLayout;
  l->setContentsMargins(0, 0, 0, 0);  // get rid of unwanted margins on layout
  l->addWidget(m_path);
  l->addWidget(m_pb_pick);

  setLayout(l);
  setContentsMargins(0, 0, 0, 0);   // get rid of unwanted margins on the widget itself
}


/**
 */
void CPathPicker::handlePickBtnClicked(void)
{
  QFileInfo info(m_path->text());

  if (m_mode == PICK_DIRS)
  {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Pick a directory"), info.absolutePath());
    if (!dir.isNull())
    {
      m_path->setText(dir);
    }
  }
  else
  {
    QString file = QFileDialog::getOpenFileName(this, tr("Pick a file"), info.absolutePath());
    if (!file.isNull())
    {
      m_path->setText(file);
    }
  }

  return;
}

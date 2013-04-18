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

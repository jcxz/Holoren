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

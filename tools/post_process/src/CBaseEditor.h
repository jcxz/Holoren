/**
 */

#ifndef CBASEEDITOR_H
#define CBASEEDITOR_H

#include <QWidget>
#include <QString>


/**
 * A base class that implements function common to all config editors
 */
class CBaseEditor : public QWidget
{
    Q_OBJECT

  public:
    /**
     * Set the path to DFtoHologram binary
     */
    void setBinary(const QString & path)
    {
      m_binary = path;
      return;
    }

  protected:
    /**
     * Default constructor
     */
    explicit CBaseEditor(const QString & binary = QString(), QWidget *parent = 0)
      : QWidget(parent),
        m_binary(binary)
    {
    }

  protected:
    /**
     * A method to run the binary associated with the DFtoHologram config file
     */
    bool run(const QString & config_file);

  private:
    QString m_binary;  /// a path to binary that will be executed
};

#endif

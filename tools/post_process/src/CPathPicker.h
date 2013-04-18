/**
 */

#ifndef CPATHPICKER_H
#define CPATHPICKER_H

#include <QWidget>
#include <QLineEdit>


class QPushButton;

/**
 * A class with widgets to pick a directory or a file
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
    explicit CPathPicker(EMode mode = PICK_FILES, const QString & path = QString(), QWidget *parent = 0);

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
    QString getPath(void) const
    {
      return m_path->text();
    }

    /**
     * A method to retrieve the selected path
     */
    void setPath(const QString & path)
    {
      m_path->setText(path);
      return;
    }

  private slots:
    /**
     */
    void handlePickBtnClicked(void);

  private:
    EMode m_mode;            /// picker mode
    QLineEdit *m_path;       /// an edit box with the picked path
    QPushButton *m_pb_pick;  /// a button to change the selection
};

#endif // CPATHPICKER_H

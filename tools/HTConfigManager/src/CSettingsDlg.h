/**
 */

#ifndef CSETTINGSDLG_H
#define CSETTINGSDLG_H

#include <QDialog>
#include <QTextEdit>
#include <QString>


class QDialogButtonBox;

/**
 * A dialog with settings
 */
class CSettingsDlg : public QDialog
{
    Q_OBJECT

  public:
    /**
     * Default constructor
     */
    explicit CSettingsDlg(const QString & settings_file = QString(), QWidget *parent = 0);

    QString getStartScript(void) const
    {
      return m_start_script->toPlainText();
    }

    bool load(const QString & filename);
    bool save(const QString & filename);

  signals:
    void error(QString error);
    void saved(void);

  private slots:
    void handleAccept(void);

  private:
    QString m_settings_file;      /// a file with settings
    QTextEdit *m_start_script;    /// a text box with console start script
    QDialogButtonBox *m_buttons;  /// ok/cancel buttons
};

#endif // CSETTINGSDLG_H

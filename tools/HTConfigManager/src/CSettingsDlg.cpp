/**
 */

#include "CSettingsDlg.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>



/**
 */
CSettingsDlg::CSettingsDlg(const QString & settings_file, QWidget *parent)
  : QDialog(parent),
    m_settings_file(settings_file),
    m_start_script(0),
    m_buttons(0)
{
  m_start_script = new QTextEdit(this);

  QVBoxLayout *l1 = new QVBoxLayout;
  l1->addWidget(m_start_script);

  QGroupBox *g = new QGroupBox;
  g->setTitle(tr("Initial Console script"));
  g->setLayout(l1);

  /* Add dialog buttons */
  m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  m_buttons->setCenterButtons(true);
  connect(m_buttons, SIGNAL(rejected()), SLOT(reject()));
  connect(m_buttons, SIGNAL(accepted()), SLOT(handleAccept()));
  connect(m_buttons, SIGNAL(accepted()), SLOT(accept()));

  /* Create layout for widgets in the dialog */
  QVBoxLayout *l2 = new QVBoxLayout;
  l2->addWidget(g);
  l2->addWidget(m_buttons);

  setLayout(l2);

  /* set layout and other properties of the main dialog */
  setWindowTitle(tr("Settings"));

  if (!load(m_settings_file))
  {
    qDebug() << "Failed to load settings from: " << settings_file;
  }
}


/**
 */
bool CSettingsDlg::load(const QString & filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
  {
    emit error(tr("Failed load settings from %1: Error opening file").arg(filename));
    return false;
  }

  QXmlStreamReader reader(&file);

  if (reader.readNext() != QXmlStreamReader::StartDocument)
  {
    emit error(tr("Failed load settings from %1: Missing start element.").arg(filename));
    return false;
  }

  if ((!reader.readNextStartElement()) || (reader.name() != "Settings"))
  {
    emit error(tr("Failed load settings from %1: Root element must be settings").arg(filename));
    return false;
  }

  if ((!reader.readNextStartElement()) || (reader.name() == "StartScript"))
  {
    m_start_script->setText(reader.readElementText());
  }

  if (reader.hasError())
  {
    emit error(tr("Failed load settings from %1: %2").arg(filename).arg(reader.errorString()));
    return false;
  }

  return true;
}


/**
 */
bool CSettingsDlg::save(const QString & filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly))
  {
    emit error(tr("Failed save settings to %1: Error creating file").arg(filename));
    return false;
  }

  QXmlStreamWriter writer(&file);

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  writer.writeStartDocument();
  writer.writeStartElement("Settings");

  writer.writeTextElement("StartScript", m_start_script->toPlainText());

  writer.writeEndElement();
  writer.writeEndDocument();

  emit saved();

  return true;
}


/**
 */
void CSettingsDlg::handleAccept(void)
{
  save(m_settings_file);
  return;
}

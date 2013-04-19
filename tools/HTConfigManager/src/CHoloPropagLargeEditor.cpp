/**
 */

#include "CHoloPropagLargeEditor.h"
#include "CPathPicker.h"
#include "CImageViewer.h"

#include <climits>

#include <QDebug>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QTextEdit>

#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>

#include <QProcess>



/**
 */
CHoloPropagLargeEditor::CHoloPropagLargeEditor(const QString & hist_file, QWidget *parent)
  : QWidget(parent),
    m_HoloPropagLarge_proc(),
    m_caption(0),
    m_cfg_file(0),
    m_binary_file(0),
    m_pb_run(0),
    m_pb_reload(0),
    m_pb_save(0),
    m_HoloPropagLarge_output(0),
    m_entry_Lambda(0),
    m_entry_Pitch(0),
    m_entry_ForceSettings(0),
    m_entry_RandomizePhase(0),
    m_entry_ImageContainsIntensity(0),
    m_entry_ImageContainsAmplitude(0),
    m_entry_ImageContainsPhase(0),
    m_entry_Input(0),
    m_entry_Target(0),
    m_entry_ImageIntensityMaximum(0),
    m_entry_ImageIntensityMinimum(0),
    m_entry_Frame(0),
    m_entry_FrameX(0),
    m_entry_FrameY(0),
    m_entry_FrameRemove(0),
    m_entry_Operations(0)
{
  /* set up DFtoHologram process */
  m_HoloPropagLarge_proc.setProcessChannelMode(QProcess::MergedChannels);  // redirect stderr to stdout
  connect(&m_HoloPropagLarge_proc, SIGNAL(readyReadStandardOutput()), SLOT(handleStdoutRead()));
  connect(&m_HoloPropagLarge_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(handleProcessError(QProcess::ProcessError)));
  connect(&m_HoloPropagLarge_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(handleProcessFinished(int, QProcess::ExitStatus)));

  /* create UI */
  m_caption = new QLabel(tr("HoloPropagLarge Editor"));
  m_caption->setAlignment(Qt::AlignCenter);
  m_caption->setStyleSheet("font-size: 16px; font-weight: bold;");

  m_HoloPropagLarge_output = new QTextEdit;

  QVBoxLayout *vl = new QVBoxLayout;
  vl->addSpacing(20);
  vl->addWidget(m_caption);
  vl->addSpacing(30);
  vl->addWidget(createSystemInfo());
  vl->addWidget(createGroupEntries());
  vl->addWidget(createGroupImageContains());
  vl->addWidget(createGroupFrame());
  vl->addWidget(createGroupOperations());
  vl->addWidget(m_HoloPropagLarge_output, 1);
  vl->addLayout(createButtons());
  setLayout(vl);

  if (!hist_file.isNull())
  {
    /* reload the config' history */
    loadHistory(hist_file);

    /* realod the first config file */
    reload();
  }

  /* connect a signal for automatic reloading each time a
     new config is picked */
  connect(m_cfg_file, SIGNAL(picked()), SLOT(reload()));
}


/**
 */
QGroupBox *CHoloPropagLargeEditor::createSystemInfo(void)
{
  /* create widgets */
  m_cfg_file = new CPathPicker;
  m_binary_file = new CPathPicker;
  m_working_dir = new CPathPicker(CPathPicker::PICK_DIRS);
  m_show_output = new QCheckBox;
  m_show_output->setChecked(true);

  /* put them in layout */
  QFormLayout *fl = new QFormLayout;
  fl->addRow(new QLabel(tr("Config file location:")),              m_cfg_file);
  fl->addRow(new QLabel(tr("Path to HoloPropagLarge:")),           m_binary_file);
  fl->addRow(new QLabel(tr("HoloPropagLarge working directory:")), m_working_dir);
  fl->addRow(new QLabel(tr("Show utility output:")),               m_show_output);

  /* put the settings in a group box */
  QGroupBox *g = new QGroupBox;
  g->setLayout(fl);

  return g;
}


/**
 */
QGroupBox *CHoloPropagLargeEditor::createGroupEntries(void)
{
  /* create widgets */
  m_entry_Lambda = new QDoubleSpinBox;
  m_entry_Lambda->setDecimals(12);

  m_entry_Pitch = new QDoubleSpinBox;
  m_entry_Pitch->setDecimals(12);

  m_entry_Input = new QLineEdit;
  m_entry_Target = new QLineEdit;
  m_entry_ForceSettings = new QCheckBox;
  m_entry_RandomizePhase = new QCheckBox;

  /* put them in layout */
  QFormLayout *fl = new QFormLayout;
  fl->addRow(new QLabel(tr("Lambda:")),         m_entry_Lambda);
  fl->addRow(new QLabel(tr("Pitch:")),          m_entry_Pitch);
  fl->addRow(new QLabel(tr("Input:")),          m_entry_Input);
  fl->addRow(new QLabel(tr("Target:")),         m_entry_Target);
  fl->addRow(new QLabel(tr("ForceSettings:")),  m_entry_ForceSettings);
  fl->addRow(new QLabel(tr("RandomizePhase:")), m_entry_RandomizePhase);

  /* put the settings in a group box */
  QGroupBox *g = new QGroupBox;
  g->setLayout(fl);

  return g;
}


/**
 */
QGroupBox *CHoloPropagLargeEditor::createGroupImageContains(void)
{
  m_entry_ImageContainsIntensity = new QCheckBox(tr("ImageContainsIntensity"));
  m_entry_ImageContainsAmplitude = new QCheckBox(tr("ImageContainsAmplitude"));
  m_entry_ImageContainsPhase = new QCheckBox(tr("ImageContainsPhase"));

  m_entry_ImageIntensityMaximum = new QDoubleSpinBox;
  m_entry_ImageIntensityMaximum->setDecimals(6);

  m_entry_ImageIntensityMinimum = new QDoubleSpinBox;
  m_entry_ImageIntensityMinimum->setDecimals(6);

  QHBoxLayout *hl = new QHBoxLayout;
  hl->addWidget(m_entry_ImageContainsIntensity);
  hl->addWidget(m_entry_ImageContainsAmplitude);
  hl->addWidget(m_entry_ImageContainsPhase);

  QFormLayout *fl = new QFormLayout;
  fl->addRow(new QLabel(tr("ImageIntensityMaximum:")), m_entry_ImageIntensityMaximum);
  fl->addRow(new QLabel(tr("ImageIntensityMinimum:")), m_entry_ImageIntensityMinimum);

  QVBoxLayout *vl = new QVBoxLayout;
  vl->addLayout(hl);
  vl->addLayout(fl);

  QGroupBox *g = new QGroupBox;
  g->setLayout(vl);

  return g;
}


/**
 */
QGroupBox *CHoloPropagLargeEditor::createGroupFrame(void)
{
  m_entry_Frame = new QCheckBox;
  m_entry_FrameX = new QSpinBox;
  m_entry_FrameX->setMaximum(INT_MAX);

  m_entry_FrameY = new QSpinBox;
  m_entry_FrameY->setMaximum(INT_MAX);

  m_entry_FrameRemove = new QCheckBox;

  QFormLayout *fl = new QFormLayout;
  fl->addRow(new QLabel(tr("Frame:")),       m_entry_Frame);
  fl->addRow(new QLabel(tr("FrameX:")),      m_entry_FrameX);
  fl->addRow(new QLabel(tr("FrameY:")),      m_entry_FrameY);
  fl->addRow(new QLabel(tr("FrameRemove:")), m_entry_FrameRemove);

  QGroupBox *g = new QGroupBox;
  g->setLayout(fl);

  return g;
}


/**
 */
QGroupBox *CHoloPropagLargeEditor::createGroupOperations(void)
{
  m_entry_Operations = new QTextEdit;

  QVBoxLayout *vl = new QVBoxLayout;
  vl->addWidget(m_entry_Operations);

  QGroupBox *g = new QGroupBox;
  g->setTitle("Operations");
  g->setLayout(vl);

  return g;
}


/**
 */
QLayout *CHoloPropagLargeEditor::createButtons(void)
{
  m_pb_run = new QPushButton(tr("Verify"));
  connect(m_pb_run, SIGNAL(clicked()), SLOT(run()));

  m_pb_reload = new QPushButton(tr("Reload"));
  connect(m_pb_reload, SIGNAL(clicked()), SLOT(reload()));

  m_pb_save = new QPushButton(tr("Save"));
  connect(m_pb_save, SIGNAL(clicked()), SLOT(save()));

  QHBoxLayout *hl = new QHBoxLayout;
  hl->addWidget(m_pb_run);
  hl->addWidget(m_pb_reload);
  hl->addWidget(m_pb_save);

  return hl;
}


/**
 * A method to load the history of config files
 */
bool CHoloPropagLargeEditor::loadHistory(const QString & hist_file)
{
  QFile file(hist_file);
  QXmlStreamReader reader(&file);

  if (!file.open(QIODevice::ReadOnly))
  {
    emit error(tr("Failed to load HoloPropagLargeEditor history file %1: Failed to open file").arg(hist_file));
    return false;
  }

  if (reader.readNext() != QXmlStreamReader::StartDocument)
  {
    qDebug() << reader.tokenString();
    emit error(tr("Failed to load HoloPropagLargeEditor history file %1: Missing start document").arg(m_cfg_file->getSelectedPath()));
    return false;
  }

  if ((!reader.readNextStartElement()) || (reader.name() != "HoloPropagLargeEditor"))
  {
    emit error(tr("Failed to load HoloPropagLargeEditor history file %1: root element is not 'HoloPropagLargeEditor'").arg(m_cfg_file->getSelectedPath()));
    return false;
  }

  /* load config files history */
  while (reader.readNextStartElement())
  {
    if (reader.name() == "CfgFiles")
    {
      m_cfg_file->clearAllPaths();
      while ((reader.readNextStartElement()) && (reader.name() == "File"))
      {
        m_cfg_file->addPath(reader.readElementText().trimmed());
      }
    }
    else if (reader.name() == "BinFiles")
    {
      m_binary_file->clearAllPaths();
      while ((reader.readNextStartElement()) && (reader.name() == "File"))
      {
        m_binary_file->addPath(reader.readElementText().trimmed());
      }
    }
    else if (reader.name() == "WorkingDirs")
    {
      m_working_dir->clearAllPaths();
      while ((reader.readNextStartElement()) && (reader.name() == "Dir"))
      {
        m_working_dir->addPath(reader.readElementText().trimmed());
      }
    }
    else if (reader.name() == "ShowOutput")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_show_output->setChecked(true);
      }
      else if (text == "false")
      {
        m_show_output->setChecked(false);
      }
      else
      {
        emit error(tr("Failed to load HoloPropagLargeEditor history file %1: error converting ShowOuput to boolean").arg(m_cfg_file->getSelectedPath()));
        return false;
      }
    }
    else
    {
      emit error(tr("Failed to load HoloPropagLargeEditor history file: Unknown field %1").arg(reader.name().toString()));
      return false;
    }
  }

  if (reader.hasError())
  {
    emit error(tr("Failed to load HoloPropagLargeEditor history file %1: error while reading XML").arg(m_cfg_file->getSelectedPath()));
    return false;
  }

  return true;
}


/**
 * A method to set the list of current config files
 */
bool CHoloPropagLargeEditor::saveHistory(const QString & hist_file)
{
  QFile file(hist_file);
  QXmlStreamWriter writer(&file);

  if (!file.open(QIODevice::WriteOnly))
  {
    emit error(tr("Failed to save HoloPropagLarge history to file %1: Failed to create file").arg(hist_file));
    return false;
  }

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  writer.writeStartDocument();
  writer.writeStartElement("HoloPropagLargeEditor");

  /* config files */
  writer.writeStartElement("CfgFiles");

  CPathPickerIterator cfg_it = m_cfg_file->getPathsIterator();
  while (cfg_it.hasNext())
  {
    writer.writeTextElement("File", cfg_it.getPath());
    cfg_it.next();
  }

  writer.writeEndElement();

  /* binary files */
  writer.writeStartElement("BinFiles");

  CPathPickerIterator bin_it = m_binary_file->getPathsIterator();
  while (bin_it.hasNext())
  {
    writer.writeTextElement("File", bin_it.getPath());
    bin_it.next();
  }

  writer.writeEndElement();

  /* working dirs */
  writer.writeStartElement("WorkingDirs");

  CPathPickerIterator dir_it = m_working_dir->getPathsIterator();
  while (dir_it.hasNext())
  {
    writer.writeTextElement("Dir", dir_it.getPath());
    dir_it.next();
  }

  writer.writeEndElement();

  writer.writeTextElement("ShowOutput", (m_show_output->isChecked() ? "true" : "false"));

  writer.writeEndElement();
  writer.writeEndDocument();

  file.close();

  return false;
}


/**
 */
bool CHoloPropagLargeEditor::run(void)
{
  qDebug() << "running HoloPropagLarge process";

  m_HoloPropagLarge_output->clear();

  /* set process arguments */
  QStringList args;
  args << ("-c" + m_cfg_file->getSelectedPath());   // must be like this because the utility expects the params to be glued together

  /* set up processe's environment */
  m_HoloPropagLarge_proc.setWorkingDirectory(m_working_dir->getSelectedPath());

  /* create a process to run the DFtoHologram Utility */
  m_HoloPropagLarge_proc.start(m_binary_file->getSelectedPath(), args);

  return true;
}


/**
 */
void CHoloPropagLargeEditor::handleStdoutRead(void)
{
  m_HoloPropagLarge_output->append(m_HoloPropagLarge_proc.readAllStandardOutput());
  return;
}


/**
 */
void CHoloPropagLargeEditor::handleProcessError(QProcess::ProcessError err)
{
  switch (err)
  {
    case QProcess::FailedToStart:
      emit error(tr("Failed to start HoloPropagLarge process"));
      break;

    case QProcess::Crashed:
      emit error(tr("HoloPropagLarge process crashed"));
      break;

    case QProcess::Timedout:
      emit error(tr("HoloPropagLarge process timed out"));
      break;

    case QProcess::WriteError:
      emit error(tr("Failed to send data to HoloPropagLarge process"));
      break;

    case QProcess::ReadError:
      emit error(tr("Failed to read data from HoloPropagLarge process"));
      break;

    default:
      break;
  }

  return;
}


/**
 */
void CHoloPropagLargeEditor::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  qDebug() << "exit code == " << exitCode;

  if (exitStatus == QProcess::CrashExit)
  {
    emit error(tr("HoloPropagLarge Process crashed"));
    return;
  }

  if (exitCode != 0)
  {
    emit error(tr("Utility exited with error: %1").arg(exitCode));
    return;
  }

  if (m_show_output->isChecked())
  {
    CImageViewer *v = new CImageViewer(this);
    v->setWindowTitle(tr("Reconstructed Hologram"));
    if (v->open(m_working_dir->getSelectedPath() + m_entry_Target->text()))
    {
      v->show();
    }
    else
    {
      delete v;
      QMessageBox::critical(this, tr("HoloPropagLarge Editor"), tr("Failed to open reconstructed hologram"));
    }
  }

  emit utilityFinished();

  return;
}


/**
 * A method to load settings from a given file
 */
bool CHoloPropagLargeEditor::reload(void)
{
  qDebug("reloading");
  qDebug() << m_cfg_file->getSelectedPath();

  QFile file(m_cfg_file->getSelectedPath());
  QXmlStreamReader reader(&file);

  if (!file.open(QIODevice::ReadOnly))
  {
    emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": Cannot open input file");
    return false;
  }

  if (reader.readNext() != QXmlStreamReader::StartDocument)
  {
    qDebug() << reader.tokenString();
    emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": Missing start document");
    return false;
  }

  if ((!reader.readNextStartElement()) || (reader.name() != "Config"))
  {
    emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": root element is not 'config'");
    return false;
  }

  /* read the values of config file */
  while (reader.readNextStartElement())
  {
    QStringRef name = reader.name();

    if (name == "Lambda")
    {
      bool ok = false;
      double val = reader.readElementText().toDouble(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting Lambda to double");
        return false;
      }

      m_entry_Lambda->setValue(val);
    }
    else if (name == "Pitch")
    {
      bool ok = false;
      double val = reader.readElementText().toDouble(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting Pitch to double");
        return false;
      }

      m_entry_Pitch->setValue(val);
    }
    else if (name == "ForceSettings")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_ForceSettings->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_ForceSettings->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting ForceSettings to boolean");
        return false;
      }
    }
    else if (name == "RandomizePhase")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_RandomizePhase->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_RandomizePhase->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting RandomizePhase to boolean");
        return false;
      }
    }
    else if (name == "ImageContainsIntensity")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_ImageContainsIntensity->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_ImageContainsIntensity->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting ImageContainsIntensity to boolean");
        return false;
      }
    }
    else if (name == "ImageContainsAmplitude")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_ImageContainsAmplitude->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_ImageContainsAmplitude->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting ImageContainsAmplitude to boolean");
        return false;
      }
    }
    else if (name == "ImageContainsPhase")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_ImageContainsPhase->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_ImageContainsPhase->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting ImageContainsPhase to boolean");
        return false;
      }
    }
    else if (name == "Input")
    {
      m_entry_Input->setText(reader.readElementText().trimmed());
    }
    else if (name == "Target")
    {
      m_entry_Target->setText(reader.readElementText().trimmed());
    }
    else if (name == "ImageIntensityMaximum")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting ImageIntensityMaximum to int");
        return false;
      }

      m_entry_ImageIntensityMaximum->setValue(val);
    }
    else if (name == "ImageIntensityMinimum")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting ImageIntensityMinimum to int");
        return false;
      }

      m_entry_ImageIntensityMinimum->setValue(val);
    }
    else if (name == "Frame")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_Frame->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_Frame->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting Frame to boolean");
        return false;
      }
    }
    else if (name == "FrameX")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting FrameX to int");
        return false;
      }

      m_entry_FrameX->setValue(val);
    }
    else if (name == "FrameY")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting FrameY to int");
        return false;
      }

      m_entry_FrameY->setValue(val);
    }
    else if (name == "FrameRemove")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_FrameRemove->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_FrameRemove->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting FrameRemove to boolean");
        return false;
      }
    }
    else if (name == "Operations")
    {
      m_entry_Operations->setText(reader.readElementText().trimmed());  // note trimmed() to remove leading and trailing whitespace
    }
    else
    {
      emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": Unknown element " + reader.name().toString());
      return false;
    }
  }

  if (reader.hasError())
  {
    emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error while reading XML");
    return false;
  }

  return false;
}


/**
 * A method to save the settings
 */
bool CHoloPropagLargeEditor::save(void)
{
  QFile file(m_cfg_file->getSelectedPath());
  QXmlStreamWriter writer(&file);

  if (!file.open(QIODevice::WriteOnly))
  {
    emit error("Failed to save config to file " + m_cfg_file->getSelectedPath() + ": Cannot open output file");
    return false;
  }

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  writer.writeStartDocument();
  writer.writeStartElement("Config");

  writer.writeTextElement("Lambda",                 QString().number(m_entry_Lambda->value()));
  writer.writeTextElement("Pitch",                  QString().number(m_entry_Pitch->value()));
  writer.writeTextElement("ForceSettings",          (m_entry_ForceSettings->isChecked() ? "true" : "false"));
  writer.writeTextElement("RandomizePhase",         (m_entry_RandomizePhase->isChecked() ? "true" : "false"));
  writer.writeTextElement("ImageContainsIntensity", (m_entry_ImageContainsIntensity->isChecked() ? "true" : "false"));
  writer.writeTextElement("ImageContainsAmplitude", (m_entry_ImageContainsAmplitude->isChecked() ? "true" : "false"));
  writer.writeTextElement("ImageContainsPhase",     (m_entry_ImageContainsPhase->isChecked() ? "true" : "false"));
  writer.writeTextElement("Input",                  m_entry_Input->text());
  writer.writeTextElement("Target",                 m_entry_Target->text());
  writer.writeTextElement("ImageIntensityMaximum",  QString().number(m_entry_ImageIntensityMaximum->value()));
  writer.writeTextElement("ImageIntensityMinimum",  QString().number(m_entry_ImageIntensityMinimum->value()));
  writer.writeTextElement("Frame",                  (m_entry_Frame->isChecked() ? "true" : "false"));
  writer.writeTextElement("FrameX",                 QString().number(m_entry_FrameX->value()));
  writer.writeTextElement("FrameY",                 QString().number(m_entry_FrameY->value()));
  writer.writeTextElement("FrameRemove",            (m_entry_FrameRemove->isChecked() ? "true" : "false"));
  writer.writeTextElement("Operations",             m_entry_Operations->toPlainText());

  writer.writeEndElement();
  writer.writeEndDocument();

  return false;
}

/**
 */

#include "CDFtoHologramEditor.h"
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
CDFtoHologramEditor::CDFtoHologramEditor(const QString & hist_file, QWidget *parent)
  : QWidget(parent),
    m_DFtoHologram_proc(),
    m_caption(0),
    m_cfg_file(0),
    m_binary_file(0),
    m_pb_run(0),
    m_pb_reload(0),
    m_pb_save(0),
    m_DFtoHologram_output(0),
    m_entry_ProcessLineCnt(0),
    m_entry_KsiXDeg(0),
    m_entry_KsiYDeg(0),
    m_entry_Input(0),
    m_entry_Target(0),
    m_entry_UsePhase(0),
    m_entry_HologramInterference(0),
    m_entry_HologramBipolar(0),
    m_entry_PsDPI(0),
    m_entry_PsDotSize(0),
    m_entry_Binarize(0),
    m_entry_Halftonize(0),
    m_entry_HalftonizeCellSize(0),
    m_entry_HalftonizeLevelCount(0),
    m_entry_HalftonizeLevelByPixel(0)
{
  /* set up DFtoHologram process */
  m_DFtoHologram_proc.setProcessChannelMode(QProcess::MergedChannels);  // redirect stderr to stdout
  connect(&m_DFtoHologram_proc, SIGNAL(readyReadStandardOutput()), SLOT(handleStdoutRead()));
  connect(&m_DFtoHologram_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(handleProcessError(QProcess::ProcessError)));
  connect(&m_DFtoHologram_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(handleProcessFinished(int, QProcess::ExitStatus)));

  /* create UI */
  m_caption = new QLabel(tr("DFtoHologram Editor"));
  m_caption->setAlignment(Qt::AlignCenter);
  m_caption->setStyleSheet("font-size: 16px; font-weight: bold;");

  m_DFtoHologram_output = new QTextEdit;

  QVBoxLayout *vl = new QVBoxLayout;
  vl->addSpacing(20);
  vl->addWidget(m_caption);
  vl->addSpacing(30);
  vl->addWidget(createSystemInfo());
  vl->addWidget(createGroupEntries());
  vl->addWidget(createGroupInterferenceBipolar());
  vl->addWidget(createGroupBinarizeHalftonize());
  //vl->addStretch();
  vl->addWidget(m_DFtoHologram_output, 1);
  vl->addLayout(createButtons());

  setLayout(vl);

  if (!hist_file.isNull())
  {
    /* reload history */
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
QGroupBox *CDFtoHologramEditor::createSystemInfo(void)
{
  /* create widgets */
  m_cfg_file = new CPathPicker;
  m_binary_file = new CPathPicker;
  m_working_dir = new CPathPicker(CPathPicker::PICK_DIRS);
  m_show_output = new QCheckBox;
  m_show_output->setChecked(true);

  /* put them in layout */
  QFormLayout *fl = new QFormLayout;
  fl->addRow(new QLabel(tr("Config file location:")),           m_cfg_file);
  fl->addRow(new QLabel(tr("Path to DFtoHologram:")),           m_binary_file);
  fl->addRow(new QLabel(tr("DFtoHologram working directory:")), m_working_dir);
  fl->addRow(new QLabel(tr("Show utility output:")),            m_show_output);

  /* put the settings in a group box */
  QGroupBox *g = new QGroupBox;
  g->setLayout(fl);

  return g;
}


/**
 */
QGroupBox *CDFtoHologramEditor::createGroupEntries(void)
{
  /* create widgets */
  m_entry_ProcessLineCnt = new QSpinBox;
  m_entry_ProcessLineCnt->setMaximum(INT_MAX);

  m_entry_KsiXDeg = new QDoubleSpinBox;
  m_entry_KsiXDeg->setDecimals(6);

  m_entry_KsiYDeg = new QDoubleSpinBox;
  m_entry_KsiYDeg->setDecimals(6);

  m_entry_Input = new QLineEdit;
  m_entry_Target = new QLineEdit;

  m_entry_PsDPI = new QSpinBox;
  m_entry_PsDPI->setMaximum(INT_MAX);

  m_entry_PsDotSize = new QSpinBox;
  m_entry_PsDotSize->setMaximum(INT_MAX);

  m_entry_UsePhase = new QCheckBox;

  /* put them in layout */
  QFormLayout *fl = new QFormLayout;
  fl->addRow(new QLabel(tr("ProcessLineCnt:")),  m_entry_ProcessLineCnt);
  fl->addRow(new QLabel(tr("KsiXDeg:")),         m_entry_KsiXDeg);
  fl->addRow(new QLabel(tr("KsiYDeg:")),         m_entry_KsiYDeg);
  fl->addRow(new QLabel(tr("Input:")),           m_entry_Input);
  fl->addRow(new QLabel(tr("Target:")),          m_entry_Target);
  fl->addRow(new QLabel(tr("PsDPI:")),           m_entry_PsDPI);
  fl->addRow(new QLabel(tr("PsDotSize:")),       m_entry_PsDotSize);
  fl->addRow(new QLabel(tr("UsePhase:")),        m_entry_UsePhase);

  /* put the settings in a group box */
  QGroupBox *g = new QGroupBox;
  g->setLayout(fl);

  return g;
}


/**
 */
QGroupBox *CDFtoHologramEditor::createGroupInterferenceBipolar(void)
{
  m_entry_HologramInterference = new QRadioButton("HologramInterference");
  m_entry_HologramInterference->setChecked(true);
  m_entry_HologramBipolar = new QRadioButton("HologramBipolar");

  QHBoxLayout *l = new QHBoxLayout;
  l->addWidget(m_entry_HologramInterference);
  l->addWidget(m_entry_HologramBipolar);

  QGroupBox *g = new QGroupBox;
  g->setLayout(l);

  return g;
}


/**
 */
QGroupBox *CDFtoHologramEditor::createGroupBinarizeHalftonize(void)
{
  //m_entry_Binarize = new QRadioButton(tr("Binarize"));
  //m_entry_Halftonize = new QRadioButton(tr("Halftonize"));

  m_entry_Binarize = new QCheckBox(tr("Binarize"));
  m_entry_Halftonize = new QCheckBox(tr("Halftonize"));
  m_entry_Halftonize->setChecked(true);

  m_entry_HalftonizeCellSize = new QSpinBox;
  m_entry_HalftonizeCellSize->setMaximum(INT_MAX);

  m_entry_HalftonizeLevelCount = new QSpinBox;
  m_entry_HalftonizeLevelCount->setMaximum(INT_MAX);

  m_entry_HalftonizeLevelByPixel = new QCheckBox;

  QHBoxLayout *hl = new QHBoxLayout;
  hl->addWidget(m_entry_Halftonize);
  hl->addWidget(m_entry_Binarize);

  QFormLayout *fl = new QFormLayout;
  fl->addRow(new QLabel(tr("HalftonizeCellSize:")), m_entry_HalftonizeCellSize);
  fl->addRow(new QLabel(tr("HalftonizeCellLevelCount:")), m_entry_HalftonizeLevelCount);
  fl->addRow(new QLabel(tr("HalftonizeLevelByPixel:")), m_entry_HalftonizeLevelByPixel);

  //QGroupBox *g1 = new QGroupBox(tr("Halftonize settings"));
  QGroupBox *g1 = new QGroupBox;
  g1->setLayout(fl);

  QVBoxLayout *vl = new QVBoxLayout;
  vl->addLayout(hl);
  vl->addWidget(g1);

  QGroupBox *g = new QGroupBox;
  g->setLayout(vl);

  return g;
}


/**
 */
QLayout *CDFtoHologramEditor::createButtons(void)
{
  m_pb_run = new QPushButton(tr("Post Process"));
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
bool CDFtoHologramEditor::loadHistory(const QString & hist_file)
{
  QFile file(hist_file);
  QXmlStreamReader reader(&file);

  if (!file.open(QIODevice::ReadOnly))
  {
    emit error(tr("Failed to load DFtoHologramEditor history file %1: Failed to open file").arg(hist_file));
    return false;
  }

  if (reader.readNext() != QXmlStreamReader::StartDocument)
  {
    qDebug() << reader.tokenString();
    emit error(tr("Failed to load DFtoHologramEditor history file %1: Missing start document").arg(m_cfg_file->getSelectedPath()));
    return false;
  }

  if ((!reader.readNextStartElement()) || (reader.name() != "DFtoHologramEditor"))
  {
    emit error(tr("Failed to load DFtoHologramEditor history file %1: root element is not 'DFtoHologramEditor'").arg(m_cfg_file->getSelectedPath()));
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
        emit error(tr("Failed to load DFtoHologramEditor history file %1: error converting ShowOutput to boolean").arg(m_cfg_file->getSelectedPath()));
        return false;
      }
    }
    else
    {
      emit error(tr("Failed to load DFtoHologramEditor history file: Unknown field %1").arg(reader.name().toString()));
      return false;
    }
  }

  if (reader.hasError())
  {
    emit error(tr("Failed to load DFtoHologramEditor history file %1: error while reading XML").arg(m_cfg_file->getSelectedPath()));
    return false;
  }

  return true;
}


/**
 * A method to set the list of current config files
 */
bool CDFtoHologramEditor::saveHistory(const QString & hist_file)
{
  QFile file(hist_file);
  QXmlStreamWriter writer(&file);

  if (!file.open(QIODevice::WriteOnly))
  {
    emit error(tr("Failed to save DFtoHologram history file %1: Failed to create file").arg(hist_file));
    return false;
  }

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  writer.writeStartDocument();
  writer.writeStartElement("DFtoHologramEditor");

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
bool CDFtoHologramEditor::run(void)
{
  qDebug() << "running DFtoHologram process";

  m_DFtoHologram_output->clear();

  /* set process arguments */
  QStringList args;
  args << ("-c" + m_cfg_file->getSelectedPath());   // must be like this because the utility expects the params to be glued together

  /* set up processe's environment */
  m_DFtoHologram_proc.setWorkingDirectory(m_working_dir->getSelectedPath());

  /* create a process to run the DFtoHologram Utility */
  m_DFtoHologram_proc.start(m_binary_file->getSelectedPath(), args);

  return true;
}


/**
 */
void CDFtoHologramEditor::handleStdoutRead(void)
{
  m_DFtoHologram_output->append(m_DFtoHologram_proc.readAllStandardOutput());
  return;
}


/**
 */
void CDFtoHologramEditor::handleProcessError(QProcess::ProcessError err)
{
  switch (err)
  {
    case QProcess::FailedToStart:
      emit error(tr("Failed to start DFtoHologram process"));
      break;

    case QProcess::Crashed:
      emit error(tr("DFtoHologram process crashed"));
      break;

    case QProcess::Timedout:
      emit error(tr("DFtoHologram process timed out"));
      break;

    case QProcess::WriteError:
      emit error(tr("Failed to send data to DFtoHologram process"));
      break;

    case QProcess::ReadError:
      emit error(tr("Failed to read data from DFtoHologram process"));
      break;

    default:
      break;
  }

  return;
}


/**
 */
void CDFtoHologramEditor::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  qDebug() << "exit code == " << exitCode;

  if (exitStatus == QProcess::CrashExit)
  {
    emit error(tr("DFtoHologram Process crashed"));
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
    v->setWindowTitle(tr("Generated Hologram"));
    if (v->open(m_working_dir->getSelectedPath() + m_entry_Target->text()))
    {
      v->show();
    }
    else
    {
      delete v;
      QMessageBox::critical(this, tr("DFtoHologram Editor"), tr("Failed to open generated hologram"));
    }
  }

  emit utilityFinished();

  return;
}


/**
 * A method to load settings from a given file
 */
bool CDFtoHologramEditor::reload(void)
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

    if (name == "ProcessLineCnt")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting ProcessLineCnt to int");
        return false;
      }

      m_entry_ProcessLineCnt->setValue(val);
    }
    else if (name == "KsiXDeg")
    {
      bool ok = false;
      double val = reader.readElementText().toDouble(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting KsiXDeg to double");
        return false;
      }

      m_entry_KsiXDeg->setValue(val);
    }
    else if (name == "KsiYDeg")
    {
      bool ok = false;
      double val = reader.readElementText().toDouble(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting KsiYDeg to double");
        return false;
      }

      m_entry_KsiYDeg->setValue(val);
    }
    else if (name == "Input")
    {
      m_entry_Input->setText(reader.readElementText().trimmed());
    }
    else if (name == "Target")
    {
      m_entry_Target->setText(reader.readElementText().trimmed());
    }
    else if (name == "UsePhase")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_UsePhase->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_UsePhase->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting UsePhase to boolean");
        return false;
      }
    }
    else if (name == "HologramInterference")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_HologramInterference->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_HologramInterference->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting HologramInterference to boolean");
        return false;
      }
    }
    else if (name == "HologramBipolar")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_HologramBipolar->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_HologramBipolar->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting HologramBipolar to boolean");
        return false;
      }
    }
    else if (name == "PsDPI")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting PsDPI to int");
        return false;
      }

      m_entry_PsDPI->setValue(val);
    }
    else if (name == "PsDotSize")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting PsDotSize to int");
        return false;
      }

      m_entry_PsDotSize->setValue(val);
    }
    else if (name == "Binarize")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_Binarize->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_Binarize->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting Binarize to boolean");
        return false;
      }
    }
    else if (name == "Halftonize")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_Halftonize->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_Halftonize->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting Halftonize to boolean");
        return false;
      }
    }
    else if (name == "HalftonizeCellSize")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting HalftonizeCellSize to int");
        return false;
      }

      m_entry_HalftonizeCellSize->setValue(val);
    }
    else if (name == "HalftonizeLevelCount")
    {
      bool ok = false;
      int val = reader.readElementText().toInt(&ok);
      if (!ok)
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting HalftonizeLevelCount to int");
        return false;
      }

      m_entry_HalftonizeLevelCount->setValue(val);
    }
    else if (name == "HalftonizeLevelByPixel")
    {
      const QString text = reader.readElementText();
      if (text == "true")
      {
        m_entry_HalftonizeLevelByPixel->setChecked(true);
      }
      else if (text == "false")
      {
        m_entry_HalftonizeLevelByPixel->setChecked(false);
      }
      else
      {
        emit error("Failed to load config file from " + m_cfg_file->getSelectedPath() + ": error converting HalftonizeLevelByPixel to boolean");
        return false;
      }
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
bool CDFtoHologramEditor::save(void)
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

  writer.writeTextElement("ProcessLineCnt",         QString().number(m_entry_ProcessLineCnt->value()));
  writer.writeTextElement("KsiXDeg",                QString().number(m_entry_KsiXDeg->value()));
  writer.writeTextElement("KsiYDeg",                QString().number(m_entry_KsiYDeg->value()));
  writer.writeTextElement("Input",                  m_entry_Input->text());
  writer.writeTextElement("Target",                 m_entry_Target->text());
  writer.writeTextElement("UsePhase",               (m_entry_UsePhase->isChecked() ? "true" : "false"));
  writer.writeTextElement("HologramInterference",   (m_entry_HologramInterference->isChecked() ? "true" : "false"));
  writer.writeTextElement("HologramBipolar",        (m_entry_HologramBipolar->isChecked() ? "true" : "false"));
  writer.writeTextElement("PsDPI",                  QString().number(m_entry_PsDPI->value()));
  writer.writeTextElement("PsDotSize",              QString().number(m_entry_PsDotSize->value()));
  writer.writeTextElement("Binarize",               (m_entry_Binarize->isChecked() ? "true" : "false"));
  writer.writeTextElement("Halftonize",             (m_entry_Halftonize->isChecked() ? "true" : "false"));
  writer.writeTextElement("HalftonizeCellSize",     QString().number(m_entry_HalftonizeCellSize->value()));
  writer.writeTextElement("HalftonizeLevelCount",   QString().number(m_entry_HalftonizeLevelCount->value()));
  writer.writeTextElement("HalftonizeLevelByPixel", (m_entry_HalftonizeLevelByPixel->isChecked() ? "true" : "false"));

  writer.writeEndElement();
  writer.writeEndDocument();

  return false;
}

/**
 */

#include "CConsoleWidget.h"

#include <QKeyEvent>

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>



static const QColor g_col_user(Qt::darkGreen);


/**
 */
CConsoleWidget::CConsoleWidget(const QString & start_script, const QString & hist_file, QWidget *parent)
  : QTextEdit(parent),
    m_fixed_position(0),
    m_shell_proc(),
    m_history(),
    m_history_selected(0)
{
  setUndoRedoEnabled(false);
  connect(this, SIGNAL(cursorPositionChanged()), SLOT(handleCursorPositionChanged()));

  setFontWeight(QFont::Bold);
  setTextColor(g_col_user);

  /* QPalette p = this->palette();
     p.setColor(QPalette::Base, QColor("#D35800"));
     this->setPalette(p); */

  /* start the underlying shell process */
  m_shell_proc.setProcessChannelMode(QProcess::MergedChannels);
  connect(&m_shell_proc, SIGNAL(readyReadStandardOutput()), SLOT(handleStdoutRead()));
  connect(&m_shell_proc, SIGNAL(readyReadStandardError()), SLOT(handleStderrRead()));
  connect(&m_shell_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(handleProcessError(QProcess::ProcessError)));
  connect(&m_shell_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(handleProcessFinished(int, QProcess::ExitStatus)));

#ifdef Q_OS_WIN32
  m_shell_proc.start("cmd.exe");
#else
  m_shell_proc.start("sh");
  m_shell_proc.write("pwd\n");
#endif

  /* load history if any history file is provided */
  if (!hist_file.isNull())
  {
    if (!loadHistory(hist_file))
    {
      qDebug() << "Failed to load command history from:" << hist_file;
    }
  }

  /* run the start script */
  writeToShellProc(start_script);
}


/**
 */
bool CConsoleWidget::loadHistory(const QString & hist_file)
{
  qDebug() << "Loading console command history";

  QFile file(hist_file);
  if (!file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Failed load console history from" << hist_file << ": Error opening file";
    emit error(tr("Failed load console history from %1: Error opening file").arg(hist_file));
    return false;
  }

  QXmlStreamReader reader(&file);

  if (reader.readNext() != QXmlStreamReader::StartDocument)
  {
    qDebug() << "Failed load console history from" << hist_file << ": Missing start element.";
    emit error(tr("Failed load console history from %1: Missing start element.").arg(hist_file));
    return false;
  }

  if ((!reader.readNextStartElement()) || (reader.name() != "ConsoleHistory"))
  {
    qDebug() << "Failed load console history from " << hist_file << ": Root element must be ConsoleHistory";
    emit error(tr("Failed load console history from %1: Root element must be ConsoleHistory").arg(hist_file));
    return false;
  }

  while (reader.readNextStartElement())
  {
    if (reader.name() == "cmd")
    {
      m_history.append(reader.readElementText());
    }
    else
    {
      qDebug() << "Failed load console history from" << hist_file << ": Unknown element" <<  reader.name().toString();
      emit error(tr("Failed load console history from %1: Unknown element %2").arg(hist_file).arg(reader.name().toString()));
      return false;
    }
  }

  m_history_selected = m_history.size();

  if (reader.hasError())
  {
    qDebug() << "Failed load console history from" << hist_file << ":" << reader.errorString();
    emit error(tr("Failed load console history from %1: %2").arg(hist_file).arg(reader.errorString()));
    return false;
  }

  return true;
}


/**
 */
bool CConsoleWidget::saveHistory(const QString & hist_file)
{
  qDebug() << "Saving console command history";

  QFile file(hist_file);
  if (!file.open(QIODevice::WriteOnly))
  {
    emit error(tr("Failed save ConsoleHistory to %1: Error creating file").arg(hist_file));
    return false;
  }

  QXmlStreamWriter writer(&file);

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  writer.writeStartDocument();
  writer.writeStartElement("ConsoleHistory");

  for (QStringList::iterator it = m_history.begin(); it != m_history.end(); ++it)
  {
    writer.writeTextElement("cmd", *it);
  }

  writer.writeEndElement();
  writer.writeEndDocument();

  return true;
}


/**
 */
void CConsoleWidget::handleStdoutRead(void)
{
#if 0
  QString szOutput = m_shell_proc.readAllStandardOutput();

#ifdef Q_OS_WIN32
  QString content;

  if (m_fixed_position != 0)
  {
    content = szOutput.right(szOutput.count() - szOutput.indexOf('\n') - 1);
  }
  else
  {
    content = szOutput;
  }

  if (content.startsWith("\f\r\n"))
  {
    clear();
    insertPlainText(content.right(content.count() - 3));
  }
  else
  {
    insertPlainText(content);
  }

#else
  insertPlainText(szOutput);
#endif
  m_fixed_position = textCursor().position();
#endif

  qDebug() << "Reading stdOUT";

  setTextColor(Qt::black);
  append(m_shell_proc.readAllStandardOutput());
  m_fixed_position = textCursor().position();
  ensureCursorVisible();
  setTextColor(g_col_user);

  return;
}


/**
 */
void CConsoleWidget::handleStderrRead(void)
{
  qDebug() << "Reading stdERR";

  setTextColor(Qt::red);
  append(m_shell_proc.readAllStandardError());
  m_fixed_position = textCursor().position();
  ensureCursorVisible();
  setTextColor(g_col_user);
  return;
}


/**
 */
void CConsoleWidget::handleProcessError(QProcess::ProcessError err)
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
void CConsoleWidget::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  qDebug() << "exit code == " << exitCode;

  close();

  if (exitStatus == QProcess::CrashExit)
  {
    emit error(tr("DFtoHologram Process crashed"));
    return;
  }

  if (exitCode != 0)
  {
    qDebug() << "Utility exited with error" << exitCode;
    //emit error(tr("Utility exited with error: %1").arg(exitCode));
    //return;
  }

  emit finished();

  return;
}


/**
 */
void CConsoleWidget::handleCursorPositionChanged(void)
{
  if (textCursor().position() < m_fixed_position)
  {
    textCursor().setPosition(m_fixed_position);
  }

  return;
}


/**
 */
void CConsoleWidget::keyPressEvent(QKeyEvent *event)
{
  // accept key presses only when they are in
  // user added characters section (after the prompt)
  if (textCursor().position() < m_fixed_position)
  {
    return;
  }

  int key = event->key();

  if ((key == Qt::Key_Backspace) || (event->key() == Qt::Key_Left))
  { // make sure that we do not erase shell prompt
    if (textCursor().position() > m_fixed_position)
    {
      QTextEdit::keyPressEvent(event);
    }
  }
  else if (key == Qt::Key_Home)
  {
    QTextCursor c = textCursor();
    c.setPosition(m_fixed_position);
    setTextCursor(c);
  }
  else if (key == Qt::Key_Return)
  {
    // Key_Return je je strede klavesnice
    // Key_Enter je na numerickej klavesnici

    // vypocitaj co je prikaz pre shell a co je uz prompt
    int count = toPlainText().count() - m_fixed_position;
    if (count > 0)
    { // save only if the command is not empty
      m_history.append(toPlainText().right(count));
      m_history_selected = m_history.size();
      writeToShellProc(m_history.last());
    }
    else
    {
      writeToShellProc("");
    }
  }
  else if (key == Qt::Key_Up)
  {
    qDebug() << "Key up";

    m_history_selected--;
    if (m_history_selected < 0)
    {
      m_history_selected = 0;
    }

    qDebug() << "history_selected: " << m_history_selected;

    if (!m_history.isEmpty())
    {
      replaceUserCommand(m_history[m_history_selected]);
    }
  }
  else if (key == Qt::Key_Down)
  {
    qDebug() << "Key down";

    m_history_selected++;
    if (m_history_selected >= m_history.size())
    {
      m_history_selected = m_history.size() - 1;
    }

    qDebug() << "history_selected: " << m_history_selected;

    if (!m_history.isEmpty())
    {
      replaceUserCommand(m_history[m_history_selected]);
    }
  }
  else
  {
    QTextEdit::keyPressEvent(event);
  }

  return;
}


/**
 * A method to remove the substring from QtextEdit that
 * represents the user's command for shell
 */
void CConsoleWidget::replaceUserCommand(const QString & new_cmd)
{
  /* set the start position (it has to be done like this,
     because textCursor returns a copy of the cursor */
  QTextCursor c = textCursor();
  c.setPosition(m_fixed_position);
  setTextCursor(c);

  /* move the cusor to the end of the text */
  moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);  // move the cursor to the end of all text

  /* replace the selected text */
  textCursor().insertText(new_cmd);

  ensureCursorVisible();  // make sure the cursor is really visible

  return;
}


/**
 */
void CConsoleWidget::writeToShellProc(QString data)
{
  qDebug() << "Sending data to shell: " << data;

  m_shell_proc.write(data.toLatin1() + '\n');
#ifdef Q_OS_UNIX
  m_shell_proc.write("pwd\n");
#endif

  return;
}

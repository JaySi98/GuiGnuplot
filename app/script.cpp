#include "script.h"

Script::Script(QMainWindow* mainWindow)
{
    pMainWindow = mainWindow;
    process = new GnuplotProcess(pMainWindow);

    textEdit = new QTextEdit();
    textEdit->setFont(QFont("Consolas", 12));
    highlighter = new Highlighter(textEdit->document());

    previousText = "";
    fileName = " ";
}

Script::Script(QString fileName, QString text, QMainWindow* mainWindow)
{
    pMainWindow = mainWindow;
    process = new GnuplotProcess(pMainWindow);

    textEdit = new QTextEdit();
    textEdit->setText(text);
    textEdit->setFont(QFont("Consolas", 12));
    highlighter = new Highlighter(textEdit->document());

    this->fileName = fileName;
    previousText = text;
}

Script::~Script()
{
    delete highlighter;
    delete textEdit;

    process->stop();
    delete  process;
}


QTextEdit* Script::getTextEdit()
{
    return this->textEdit;
}

void Script::setText(QString text)
{
    this->textEdit->setText(text);
}

void Script::setFileName(QString filename)
{
    this->fileName = filename;
}

QString Script::getFileName()
{
    return this->fileName;
}

QString Script::getText()
{
    return textEdit->toPlainText();
}

void Script::setPreviousText(QString previousText)
{
    this->previousText = previousText;
}

/*!
 * @brief Checks if there are unsaved changes in text
 */
bool Script::isSaved()
{
    QString text = textEdit->toPlainText();
    if((text.compare(previousText)!= 0)  && (!text.isEmpty()))
    {
        return 0;
    }

    return 1;
}

/*!
 * @brief Appends TextEdit by new command
 */
void Script::addCommand(QString command)
{
    textEdit->append(command);
}


void Script::copy()
{
    textEdit->copy();
}

void Script::cut()
{
    textEdit->cut();
}

void Script::paste()
{
    textEdit->paste();
}

void Script::undo()
{
    textEdit->undo();
}

void Script::redo()
{
    textEdit->redo();
}



GnuplotProcess* Script::getProcess()
{
    return process;
}

QString Script::getOperatingSystem()
{
    return process->getOperatingSystem();
}

void Script::startProcess()
{
    process->start();
}

void Script::help(QString command)
{
    process->help(command);
}

void Script::compile()
{
    QString text = textEdit->toPlainText();
    if(!text.isEmpty())
    {
        //output("\nGenerating graph", OutputType::STANDARD);

        process->setScript(text);
        process->generateGraph();
    }
    else
    {
        QMessageBox::warning(pMainWindow, "Empty script",
                             "Nothing to compile",
                             QMessageBox::Ok);
    }
}

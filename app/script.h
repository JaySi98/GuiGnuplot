#ifndef SCRIPT_H
#define SCRIPT_H

#include <QtCore>
#include <QTextEdit>
#include <QMessageBox>
#include "highlighter.h"
#include "gnuplotProcess.h"
#include "QMainWindow"

class Script
{

public:
    Script(QMainWindow* mainWindow);
    Script(QString fileName, QString text, QMainWindow* mainWindow);
    ~Script();

    QTextEdit* getTextEdit();
    void       setFileName(QString filename);
    QString    getFileName();
    QString    getText();
    void       setText(QString text);
    void       setPreviousText(QString previousText);
    bool       isSaved();
    void       addCommand(QString command);

    GnuplotProcess* getProcess();
    void       startProcess();
    QString    getOperatingSystem();
    void       help(QString);
    void       compile();
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();

private:
    Highlighter* highlighter;
    QTextEdit*   textEdit;
    QString      fileName;
    QString      previousText;

    // test
    GnuplotProcess* process;
    QMainWindow*    pMainWindow;
};

#endif // SCRIPT_H

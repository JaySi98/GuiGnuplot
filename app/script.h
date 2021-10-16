#ifndef SCRIPT_H
#define SCRIPT_H

#include <QtCore>
#include <QTextEdit>
#include "highlighter.h"

class Script
{

public:
    Script();
    Script(QString fileName, QString text);
    ~Script();

    QTextEdit* getTextEdit();
    void       setFileName(QString filename);
    QString    getFileName();
    QString    getText();
    void       setText(QString text);
    void       setPreviousText(QString previousText);
    bool       isSaved();
    void       addCommand(QString command);

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
};

#endif // SCRIPT_H

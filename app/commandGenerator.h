#ifndef COMMANDGENERATOR_H
#define COMMANDGENERATOR_H

#include <QObject>
#include "QMainWindow"
#include "command.h"
#include "enums.h"
#include "lineParsing.h"
#include <QtDebug>
//
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QPushButton>
#include <QScrollArea>
#include <QLineEdit>
#include <QFileDialog>
#include <QRadioButton>


class CommandOptionsHandler : public QObject
{
    Q_OBJECT


public:
    explicit CommandOptionsHandler(QObject *parent = nullptr);
             CommandOptionsHandler(QMainWindow *main_window);

    QWidget* getCommandWidgets(QString command_name, int index);
    // test
    QWidget* generateCommandWidgets(QList<QString> settings);
    QLayout*    generateParameter(QList<QString>, QWidget *central);
    WidgetType  resolveParameter(QString input);

    QString  getCommandString();
    void     freeMemory();

    // tutaj dac znaczący komentarz
    QString     parseFileLine(QString line);

    void        createNewCommand(QString name);
    // tutaj dac znaczący komentarz
    QLayout*    generateLineEdit(QList<QString>, QWidget *central);
    QLayout*    generateCombobox(QList<QString>, QWidget *central);
    QLayout*    generateSpinbox(QList<QString>, QWidget *central);
    QLayout*    generateMultiSpinbox(QList<QString>, QWidget *central);
    QLayout*    generateDoubleSpinbox(QList<QString>, QWidget *central);
    QLayout*    generateFileSearch(QList<QString>, QWidget *central);
    QLayout*    generateNoParametersLabel(QWidget *central);

private:
    QMainWindow*   main_window;
    Command*       command;
    QList<QString> list_settings;



signals:
    void output(QString data, OutputType type);

};

#endif // COMMANDGENERATOR_H

#ifndef COMMANDGENERATOR_H
#define COMMANDGENERATOR_H

#include <QObject>
#include "QMainWindow"
#include "command.h"
#include "enums.h"
#include "lineParsing.h"
#include <QtDebug>
#include <QList>
#include <QPair>
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

#define ERROR_MESSAGE "Failed to load all parameters "

class CommandOptionsHandler : public QObject
{
    Q_OBJECT

public:
    explicit CommandOptionsHandler(QObject *parent = nullptr);
             CommandOptionsHandler(QMainWindow *main_window);

    WidgetType resolveParameter(QString input);
    QString    getCommandString();
    QString    parseFileLine(QString line);
    void       freeMemory();
    void       createNewCommand(QString name);

    QWidget*        getCommandWidgets(QString command_name, int index);
    QList<QLayout*> generateCommandWidgets(QList<QString> settings, QWidget* central);
    QLayout* generateParameter(QList<QString>, QWidget *central);
    QLayout* generateLineEdit(QList<QString>, QWidget *central);
    QLayout* generateCombobox(QList<QString>, QWidget *central);
    QLayout* generateSpinbox(QList<QString>, QWidget *central);
    QLayout* generateMultiSpinbox(QList<QString>, QWidget *central);
    QLayout* generateDoubleSpinbox(QList<QString>, QWidget *central);
    QLayout* generateFileSearch(QList<QString>, QWidget *central);
    QLayout* generateNoParametersLabel(QWidget *central);

private:
    QMainWindow*    pMainWindow;
    Command*        command;
    QList<QString>  settingsList;
    QList<QLayout*> generatedWidgets;

signals:
    void output(QString data, OutputType type);
};

#endif // COMMANDGENERATOR_H

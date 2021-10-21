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


class CommandOptionsHandler : public QObject
{
    Q_OBJECT


public:
    explicit CommandOptionsHandler(QObject *parent = nullptr);
             CommandOptionsHandler(QMainWindow *main_window);

    QWidget*   getCommandWidgets(QString command_name, int index);
    QWidget*   generateCommandWidgets(QList<QString> settings);

    QLayout*   generateParameter(QList<QString>, QWidget *central);
    WidgetType resolveParameter(QString input);
    QString  getCommandString();
    void     freeMemory();
    QString  parseFileLine(QString line);
    void     createNewCommand(QString name);
    QLayout* generateLineEdit(QList<QString>, QWidget *central);
    QLayout* generateCombobox(QList<QString>, QWidget *central);
    QLayout* generateSpinbox(QList<QString>, QWidget *central);
    QLayout* generateMultiSpinbox(QList<QString>, QWidget *central);
    QLayout* generateDoubleSpinbox(QList<QString>, QWidget *central);
    QLayout* generateFileSearch(QList<QString>, QWidget *central);
    QLayout* generateNoParametersLabel(QWidget *central);

    // test
    QWidget* GetGridLayout(QString command_name, int index);
    void GenerateWidgetList(QList<QString> settings, QWidget *central);
    QPair<QLayout*, QLayout*> GenerateWidget(QList<QString>settings, QWidget *central);

    QPair<QLayout*, QLayout*> GenerateLineEdit(QList<QString>, QWidget *central);
    QPair<QLayout*, QLayout*> GenerateCombobox(QList<QString>, QWidget *central);
    QPair<QLayout*, QLayout*> GenerateSpinbox(QList<QString>, QWidget *central);
    QPair<QLayout*, QLayout*> GenerateMultiSpinbox(QList<QString>, QWidget *central);
    QPair<QLayout*, QLayout*> GenerateDoubleSpinbox(QList<QString>, QWidget *central);
    QPair<QLayout*, QLayout*> GenerateFileSelect(QList<QString>, QWidget *central);
    QPair<QLayout*, QLayout*> GenerateNoParameters(QWidget *central);

private:
    QMainWindow*   pMainWindow;
    Command*       command;
    QList<QString> list_settings;

    // test
    QList<QPair<QLayout*, QLayout*>> generatedWidgetsList;

signals:
    void output(QString data, OutputType type);

};

#endif // COMMANDGENERATOR_H

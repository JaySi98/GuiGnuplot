#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>
#include <QList>
#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QLineEdit>
#include <QObject>
#include "QMainWindow"
#include "enums.h"

#define DONT_INCLUDE "dont include"

class Parameter: public QObject
{
     Q_OBJECT

public:
    explicit Parameter(QObject *parent = nullptr);
    Parameter(QString body, QString value);
    void    freeMemory();
    QString toQString();

    void setBody(QString body);
    void setValue(QString value);
    void setWidgets(QList<QObject*>* widgets);
    void addWidget(QObject* widget);
    void setMainWindow(QMainWindow* window);
    void setWidgetType(WidgetType type);

    QString          getBody();
    QString          getValue();
    QList<QObject*>* getWidgets();

private:
    QMainWindow*     main_window;
    QString          body;
    QString          value;
    QList<QObject*>* widgets;
    WidgetType       widget_type;

public slots:
    void stringChangeSlot(const QString &text);
    void fileSelectSlot();
    void checkBoxSlot(const int &state);
};

#endif // PARAMETER_H

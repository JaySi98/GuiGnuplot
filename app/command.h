#ifndef COMMAND_H
#define COMMAND_H

#include "parameter.h"
#include <QString>
#include <QList>
#include <QtDebug>


class Command
{
public:
    Command();
    QString toQString();
    void    freeMemory();

    QString getName();
    void    setName(QString);

    QList<Parameter*> getParameters();
    void              addParameter(Parameter* param);

private:
    QString name;
    QList<Parameter*> parameters;
};

#endif // COMMAND_H

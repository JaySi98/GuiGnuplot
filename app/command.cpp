#include "command.h"

Command::Command()
{

}

QString Command::toQString()
{
    QString output = this->name;

    for(int i = 0; i < this->parameters.count(); i++)
    {
        QString parameter = this->parameters[i]->toQString();

        if(parameter != nullptr)
        {
            output.append(" ");
            output.append(parameter);
        }
    }

    return output;
}

void Command::freeMemory()
{
    for(int i = 0; i < parameters.count(); i++)
    {
        parameters[i]->freeMemory();

        delete parameters[i];
    }
}

/* ************************** GETTERS & SETTERS **************************** */
QString Command::getName()
{
    return this->name;
}

void Command::setName(QString name)
{
    this->name = name;
}

QList<Parameter *> Command::getParameters()
{
    return this->parameters;
}

void Command::addParameter(Parameter *param)
{
    this->parameters.append(param);
}


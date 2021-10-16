#include "parameter.h"

Parameter::Parameter(QObject *parent): QObject(parent)
{

}

Parameter::Parameter(QString body, QString value)
{
    this->body = body;
    this->value = value;
}

void Parameter::freeMemory()
{
    if(widgets != nullptr)
    {
        delete widgets;
    }
 }

QString Parameter::toQString()
{
    QString output = this->body;

    if((value.compare(" ") == 0) || (value.contains(DONT_INCLUDE)))
    {
        return nullptr;
    }

    output = output.arg(value);

    return output;
}


/* ************************** GETTERS & SETTERS **************************** */
void Parameter::setBody(QString body)
{
    this->body = body;
}

void Parameter::setValue(QString value)
{
    this->value = value;
}

void Parameter::setWidgets(QList<QObject *> *widgets)
{
    this->widgets = widgets;
}

void Parameter::addWidget(QObject *widget)
{
    widgets->append(widget);
}

void Parameter::setMainWindow(QMainWindow *window)
{
    this->main_window = window;
}

void Parameter::setWidgetType(WidgetType type)
{
    this->widget_type = type;
}

QString Parameter::getBody()
{
    return this->body;
}

QString Parameter::getValue()
{
    return this->value;
}

QList<QObject *>* Parameter::getWidgets()
{
    return this->widgets;
}


/* ******************************** SLOTS ********************************** */
void Parameter::stringChangeSlot(const QString &text)
{
    if(value.contains(DONT_INCLUDE))
    {
        value = text;
        value.append(DONT_INCLUDE);
    }
    else
    {
        value = text;
    }
}

void Parameter::fileSelectSlot()
{
    QString file_open = QFileDialog::getOpenFileName(main_window, "Select file");
    if(!file_open.isEmpty())
    {
        for(int i = 0; i < widgets->count(); i++)
        {
            QObject* widget = widgets->at(i);
            qDebug() << widgets->at(i);
            if(qobject_cast<QLineEdit*>(widget))
            {
                QLineEdit* line_edit = qobject_cast<QLineEdit*>(widget);
                line_edit->setText(file_open);
            }
        }
    }
}

void Parameter::checkBoxSlot(const int &state)
{
    if(state == Qt::Unchecked)
    {
        value.append(DONT_INCLUDE);
    }
    else if(state == Qt::Checked)
    {
        value.remove(DONT_INCLUDE);
    }
}

#include "commandGenerator.h"

CommandOptionsHandler::CommandOptionsHandler(QObject *parent) : QObject(parent){}
CommandOptionsHandler::CommandOptionsHandler(QMainWindow *mainWindow)
{
    this->pMainWindow = mainWindow;
    command = new Command();
}

void CommandOptionsHandler::createNewCommand(QString name)
{
    command = new Command();
    command->setName(name);
}

/*!
 * @brief Function converts current command with selected
 * parameters to QString
 * @return QString - completed command
 */
QString CommandOptionsHandler::getCommandString()
{
    return command->toQString();
}

/*!
 * @brief after every new command is selected, this is called
 * to free memory of dynamic widgets of previous command
 */
void CommandOptionsHandler::freeMemory()
{
    command->freeMemory();
    delete command;

    generatedWidgets.clear();
}

/*!
 * @brief Function returns generted widgets for selected command
 * @param QString command_name - name of selected command
 * @param int index - line in file coresponding to selected command
 * @return QWidget*
 */
QWidget *CommandOptionsHandler::getCommandWidgets(QString commandName, int index)
{
    // get command settings and create new command
    QString line = settingsList[index];
    QList<QString> settings = splitSettings(line);
    createNewCommand(commandName);

    //QWidget *central = generateCommandWidgets(settings);
    QWidget *central = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    generatedWidgets = generateCommandWidgets(settings, central);

    for(int i = 0; i < generatedWidgets.count(); i++)
    {
        QLayout* layout = generatedWidgets[i];
        if(layout != nullptr)
        {
            mainLayout->addLayout(layout);
        }
        else
        {
            output(ERROR_MESSAGE, OutputType::ERROR);
        }
    }

    return central;
}

/*!
 * @brief Function returns widgets based on settings read from the file
 * @param QList<QString> settings - list of &p|...|| stringd
 * @return QWidget*
 */
QList<QLayout*> CommandOptionsHandler::generateCommandWidgets(QList<QString> settings, QWidget* central)
{
    QList<QLayout*> list;

    // for every "&p|..||" create new parameter
    while(!settings.empty())
    {
        QList<QString> splitedSetting = settings.takeFirst().split("|");

        // correct setting always consists of label text and widget enum
        if(splitedSetting.count() >= MIN_SETTING_NUM)
        {
            QLayout* singleParamLayout = generateParameter(splitedSetting, central);
            list.append(singleParamLayout);
        }
        else if(splitedSetting[0].contains("no parameters"))
        {
            QLayout* singleParamLayout = generateNoParametersLabel(central);
            list.append(singleParamLayout);
        }
        else
        {
            QLayout* incorrectParam = nullptr;
            list.append(incorrectParam);
        }
    }

    return list;
}

/*!
 * @brief Function returns widgets based on settings read from the file
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateParameter(QList<QString>settings, QWidget *central)
{
    switch(resolveParameter(settings[WIDGET_POS]))
    {
        case WidgetType::LINE_EDIT:      return generateLineEdit(settings, central);   break;

        case WidgetType::COMBOBOX:       return generateCombobox(settings, central);   break;

        case WidgetType::SPINBOX:        return generateSpinbox(settings, central);    break;

        case WidgetType::MULTISPINBOX:   return generateMultiSpinbox(settings, central);  break;

        case WidgetType::DOUBLE_SPINBOX: return generateDoubleSpinbox(settings, central); break;

        case WidgetType::FILE_SEARCH:    return generateFileSearch(settings, central); break;

        case WidgetType::NO_PARAMS:      return generateNoParametersLabel(central);

        case WidgetType::INVALID:        return nullptr;
    }
}

/*!
 * @brief Help function, allows to use string in switch case
 * @param QString input - type of widget to generate parameter
 * @return WidgetType
 */
WidgetType CommandOptionsHandler::resolveParameter(QString input)
{
    if(input.contains("text") )             return WidgetType::LINE_EDIT;
    if(input.contains("file selection"))    return WidgetType::FILE_SEARCH;
    if(input.contains("selection"))         return WidgetType::COMBOBOX;
    if(input.contains("float numeric"))     return WidgetType::DOUBLE_SPINBOX;
    if(input.contains("multiple numerics")) return WidgetType::MULTISPINBOX;
    if(input.contains("numeric") )          return WidgetType::SPINBOX;
    if(input.contains("no parameters"))     return WidgetType::NO_PARAMS;

    return WidgetType::INVALID;
}

/*!
 * @brief Function to geretate line edit parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateLineEdit(QList<QString> settings, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widgetLayout = new QList<QObject*>();
    Parameter* parameter = new Parameter(settings[TEXT_POS], DONT_INCLUDE);

    // top layout with label --------------------------------------------------
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label on top of rest widgets
    QLabel *label = new QLabel(settings[LABEL_POS],central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


    // inner layout with editing widgets --------------------------------------
    QHBoxLayout *innerLayout = new QHBoxLayout();
    innerLayout->setSizeConstraint(QLayout::SetFixedSize);

    QLineEdit *lineEdit = new QLineEdit(central);
    lineEdit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    lineEdit->setFixedWidth(300);

    QObject::connect(lineEdit,SIGNAL(textChanged(const QString &))
                    ,parameter,SLOT(stringChangeSlot(const QString &)));

    QSpacerItem *spacer = new QSpacerItem(100,1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    widgetLayout->append(lineEdit);
    innerLayout->addWidget(lineEdit);
    innerLayout->addSpacerItem(spacer);

    // checkbox ---------------------------------------------------------------
    QCheckBox *checkbox = new QCheckBox("include",central);
    checkbox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                    ,parameter,SLOT(checkBoxSlot(const int &)));

    widgetLayout->append(checkbox);
    innerLayout->addWidget(checkbox);


    labelLayout->addWidget(label);
    labelLayout->addLayout(innerLayout);

    // adds parameter class to command list
    parameter->setWidgets(widgetLayout);
    command->addParameter(parameter);

    return labelLayout;
}

/*!
 * @brief Function to geretate parameter parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateCombobox(QList<QString> settings, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widgetList = new QList<QObject*>();
    Parameter* parameter = new Parameter(settings[TEXT_POS], " ");

    // top layout with label --------------------------------------------------
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label on top of rest widgets
    QLabel *label = new QLabel(settings[LABEL_POS],central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


    // inner layout with editing widgets --------------------------------------
    QHBoxLayout *innerLayout = new QHBoxLayout();
    innerLayout->setSizeConstraint(QLayout::SetFixedSize);

    QComboBox *combo = new QComboBox(central);
    combo->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    combo->setFixedWidth(300);
    QList<QString> options = parseWidgetOptions(settings[WIDGET_POS]);
    parameter->setValue(options[0] + DONT_INCLUDE);
    for(int i = 0; i < options.count(); i++)
    {
        combo->addItem(options[i]);
    }
    QObject::connect(combo,SIGNAL(currentTextChanged(const QString &))
                    ,parameter,SLOT(stringChangeSlot(const QString &)));

    QSpacerItem *spacer = new QSpacerItem(100,1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    widgetList->append(combo);
    innerLayout->addWidget(combo);
    innerLayout->addSpacerItem(spacer);

    // checkbox ---------------------------------------------------------------
    QCheckBox *checkbox = new QCheckBox("include",central);
    checkbox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                    ,parameter,SLOT(checkBoxSlot(const int &)));

    widgetList->append(checkbox);
    innerLayout->addWidget(checkbox);

    labelLayout->addWidget(label);
    labelLayout->addLayout(innerLayout);

    // adds parameter class to command list
    parameter->setWidgets(widgetList);
    command->addParameter(parameter);

    return labelLayout;
}

/*!
 * @brief Function to geretate spinbox parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateSpinbox(QList<QString> settings, QWidget *central)
{
    // list to hold widgets
    QList<QObject*>* widgetList = new QList<QObject*>();
    Parameter* parameter = new Parameter(settings[TEXT_POS], QString("0%0").arg(DONT_INCLUDE));

    // top layout with label --------------------------------------------------
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label on top of rest widgets
    QLabel *label = new QLabel(settings[LABEL_POS],central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


    // inner layout with editing widgets --------------------------------------
    QHBoxLayout *innerLayout = new QHBoxLayout();
    innerLayout->setSizeConstraint(QLayout::SetFixedSize);

    QSpinBox *spinbox = new QSpinBox(central);
    spinbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spinbox->setFixedWidth(100);
    spinbox->setMaximum(1000);
    spinbox->setMinimum(-1000);
    QObject::connect(spinbox,SIGNAL(textChanged(const QString &))
                    ,parameter,SLOT(stringChangeSlot(const QString &)));

    QSpacerItem *spacer = new QSpacerItem(404, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    widgetList->append(spinbox);
    innerLayout->addWidget(spinbox);
    innerLayout->addSpacerItem(spacer);

    // checkbox ---------------------------------------------------------------
    QCheckBox *checkbox = new QCheckBox("include",central);
    checkbox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                    ,parameter,SLOT(checkBoxSlot(const int &)));

    widgetList->append(checkbox);
    innerLayout->addWidget(checkbox);

    labelLayout->addWidget(label);
    labelLayout->addLayout(innerLayout);

    // adds parameter class to command list
    parameter->setWidgets(widgetList);
    command->addParameter(parameter);

    return labelLayout;
}

/*!
 * @brief Function to geretate mulitple spiboxes in one layout
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateMultiSpinbox(QList<QString> settings, QWidget *central)
{
    // top layout with label --------------------------------------------------
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label on top of rest widgets
    QLabel *label = new QLabel(settings[LABEL_POS],central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // checkbox ---------------------------------------------------------------
    QCheckBox *checkbox = new QCheckBox("include",central);
    checkbox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);


    // inner layout with editing widgets --------------------------------------
    QHBoxLayout *innerLayout = new QHBoxLayout();
    innerLayout->setSizeConstraint(QLayout::SetFixedSize);

    int spinbox_number = parseWidgetNumber(settings[WIDGET_POS]);
    for(int i = 0; i < spinbox_number; i++)
    {
        QList<QObject*>* widget_list = new QList<QObject*>();

        // first spinbox gets parameter's body
        QString text = "%0,";
        if(i == 0)
        {
            text = settings[TEXT_POS];
        }
        // last one is without ","
        else if(i == spinbox_number -1)
        {
            text = "%0";
        }

        Parameter* parameter = new Parameter(text, QString("0%0").arg(DONT_INCLUDE));

        QSpinBox *spinbox = new QSpinBox(central);
        spinbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        spinbox->setMinimumWidth(100);
        spinbox->setMaximum(1000);
        spinbox->setMinimum(-1000);
        QObject::connect(spinbox,SIGNAL(textChanged(const QString &))
                             ,parameter,SLOT(stringChangeSlot(const QString &)));
        widget_list->append(spinbox);
        innerLayout->addWidget(spinbox);

        // connecitng checkbox to every spinbox
        QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                        ,parameter,SLOT(checkBoxSlot(const int &)));
        widget_list->append(checkbox);

        // adds parameter class to command list
        parameter->setWidgets(widget_list);
        parameter->setWidgetType(WidgetType::MULTISPINBOX);

        command->addParameter(parameter);
    }

    // spacer for astetics
    QSpacerItem *spacer = new QSpacerItem(100, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    innerLayout->addSpacerItem(spacer);
    innerLayout->addWidget(checkbox);

    labelLayout->addWidget(label);
    labelLayout->addLayout(innerLayout);

    return labelLayout;
}

/*!
 * @brief Function to geretate float spinbox parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateDoubleSpinbox(QList<QString> settings, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widgetList = new QList<QObject*>();
    Parameter* parameter = new Parameter(settings[TEXT_POS], QString("0,0%0").arg(DONT_INCLUDE));

    // top layout with label --------------------------------------------------
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label on top of rest widgets
    QLabel *label = new QLabel(settings[LABEL_POS],central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


    // inner layout with editing widgets --------------------------------------
    QHBoxLayout *innerLayout = new QHBoxLayout();
    innerLayout->setSizeConstraint(QLayout::SetFixedSize);

    QDoubleSpinBox *spinbox = new QDoubleSpinBox(central);
    spinbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spinbox->setMinimumWidth(100);
    spinbox->setMaximum(1000);
    spinbox->setMinimum(-1000);
    spinbox->setDecimals(1);
    QObject::connect(spinbox,SIGNAL(textChanged(const QString &))
                    ,parameter,SLOT(stringChangeSlot(const QString &)));

    QSpacerItem *spacer = new QSpacerItem(390, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    widgetList->append(spinbox);
    innerLayout->addWidget(spinbox);
    innerLayout->addSpacerItem(spacer);

    // checkbox ---------------------------------------------------------------
    QCheckBox *checkbox = new QCheckBox("include",central);
    checkbox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                        ,parameter,SLOT(checkBoxSlot(const int &)));
    widgetList->append(checkbox);
    innerLayout->addWidget(checkbox);


    labelLayout->addWidget(label);
    labelLayout->addLayout(innerLayout);

    // adds parameter class to command list
    parameter->setWidgets(widgetList);
    command->addParameter(parameter);

    return labelLayout;
}

/*!
 * @brief Function to geretate file selection parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateFileSearch(QList<QString> settings, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widgetList = new QList<QObject*>();
    Parameter* parameter = new Parameter(settings[TEXT_POS], DONT_INCLUDE);

    // top layout with label --------------------------------------------------
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label on top of rest widgets
    QLabel *label = new QLabel(settings[LABEL_POS],central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


    // inner layout with editing widgets --------------------------------------
    QHBoxLayout *innerLayout = new QHBoxLayout();
    innerLayout->setSizeConstraint(QLayout::SetFixedSize);

    QLineEdit *lineEdit = new QLineEdit(central);
    lineEdit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    lineEdit->setMinimumWidth(300);
    QObject::connect(lineEdit,SIGNAL(textChanged(const QString &))
                    ,parameter,SLOT(stringChangeSlot(const QString &)));

    QPushButton *button = new QPushButton("...", central);
    button->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    QObject::connect(button,&QPushButton::pressed
                     ,parameter,&Parameter::fileSelectSlot);

    QSpacerItem *spacer = new QSpacerItem(404, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    widgetList->append(lineEdit);
    widgetList->append(button);
    innerLayout->addWidget(lineEdit);
    innerLayout->addWidget(button);
    innerLayout->addSpacerItem(spacer);

    // checkbox ---------------------------------------------------------------
    QCheckBox *checkbox = new QCheckBox("include",central);
    checkbox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                    ,parameter,SLOT(checkBoxSlot(const int &)));

    widgetList->append(checkbox);
    innerLayout->addWidget(checkbox);


    labelLayout->addWidget(label);
    labelLayout->addLayout(innerLayout);

    // adds parameter class to command list
    parameter->setWidgets(widgetList);
    parameter->setMainWindow(pMainWindow);
    command->addParameter(parameter);

    return labelLayout;
}


/*!
 * @brief If command has no parameters this is used to inform about it
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateNoParametersLabel(QWidget *central)
{
    QVBoxLayout* outerLayout = new QVBoxLayout();
    outerLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label
    QLabel *label = new QLabel(NO_PARAM_MESSAGE,central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    outerLayout->addWidget(label);

    return outerLayout;
}

/*!
 * @brief  return command name from file line, adds the rest to the list
 * @param  QString line - line from "commands.txt"
 * @return QString - command name
 */
QString CommandOptionsHandler::parseFileLine(QString line)
{
    QString command_name;
    QString command_generator;

    // add command parameters to list
    command_generator = parseCommandGenerator(line);
    settingsList << command_generator;

    // get command name and remove "markers"
    command_name = parseCommandName(line);

    return command_name;
}

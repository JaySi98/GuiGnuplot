#include "commandGenerator.h"

CommandOptionsHandler::CommandOptionsHandler(QObject *parent) : QObject(parent)
{

}

CommandOptionsHandler::CommandOptionsHandler(QMainWindow *main_window)
{
    this->main_window = main_window;
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
}

/*!
 * @brief Function returns generted widgets for selected command
 * @param QString command_name - name of selected command
 * @param int index - line in file coresponding to selected command
 * @return QWidget*
 */
QWidget *CommandOptionsHandler::getCommandWidgets(QString command_name, int index)
{
    // get command settings and create new command
    QString line = list_settings[index];
    QList<QString> settings = splitSettings(line);
    createNewCommand(command_name);

    // parent widget, allows to easly delete every other widget
    QWidget *central = generateCommandWidgets(settings);
    
    return central;
}

/*!
 * @brief Function returns widgets based on settings read from the file
 * @param QList<QString> settings - list of &p|...|| stringd
 * @return QWidget*
 */
QWidget* CommandOptionsHandler::generateCommandWidgets(QList<QString> settings)
{
    // parent widget, allows to easly delete every other widget
    QWidget *central = new QWidget;
    // new layout of scroll area in main window
    QVBoxLayout *main_layout = new QVBoxLayout(central);

    // for every "&p|..||" create new parameter
    while(!settings.empty())
    {
        QList<QString> splited_setting = settings.takeFirst().split("|");

        // correct setting always consists of label text and widget enum
        if(splited_setting.count() >= MIN_SETTING_NUM)
        {
            QLayout* inner_layout = generateParameter(splited_setting, central);
            if(inner_layout != nullptr)
            {
                main_layout->addLayout(inner_layout);
            }
        }
        else
        {
            main_layout->addLayout(generateNoParametersLabel(central));
        }
    }

    return central;
}

/*!
 * @brief Function returns widgets based on settings read from the file
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateParameter(QList<QString>settings, QWidget *central)
{
    // layout to hold label
    QVBoxLayout* outer_layout = new QVBoxLayout();
    outer_layout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label on top of rest widgets
    QLabel *label = new QLabel(settings[LABEL_POS],central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    outer_layout->addWidget(label);

    //
    switch(resolveParameter(settings[WIDGET_POS]))
    {
        case WidgetType::LINE_EDIT:      outer_layout->addLayout(generateLineEdit(settings, central));   break;

        case WidgetType::COMBOBOX:       outer_layout->addLayout(generateCombobox(settings, central));   break;

        case WidgetType::SPINBOX:        outer_layout->addLayout(generateSpinbox(settings, central));    break;

        case WidgetType::MULTISPINBOX:   outer_layout->addLayout(generateMultiSpinbox(settings, central));  break;

        case WidgetType::DOUBLE_SPINBOX: outer_layout->addLayout(generateDoubleSpinbox(settings, central)); break;

        case WidgetType::FILE_SEARCH:    outer_layout->addLayout(generateFileSearch(settings, central)); break;

        case WidgetType::INVALID:
        {
            emit output("incorrect command settings", OutputType::ERROR);

            delete  outer_layout;
            delete label;

            return nullptr;
        }
    }

    return outer_layout;
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
QLayout *CommandOptionsHandler::generateLineEdit(QList<QString> input, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widget_list = new QList<QObject*>();

    Parameter* parameter = new Parameter(input[TEXT_POS], DONT_INCLUDE);

    // layout for specified widgets
    QHBoxLayout *inner_layout = new QHBoxLayout();
    inner_layout->setSizeConstraint(QLayout::SetFixedSize);

    QLineEdit *line_edit = new QLineEdit(central);
    line_edit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed); //Preferred
    //line_edit->setFixedWidth(300);

    QObject::connect(line_edit,SIGNAL(textChanged(const QString &))
                     ,parameter,SLOT(stringChangeSlot(const QString &)));
    widget_list->append(line_edit);
    inner_layout->addWidget(line_edit);

    QSpacerItem *spacer = new QSpacerItem(100,1, QSizePolicy::Fixed, QSizePolicy::Fixed);
    inner_layout->addSpacerItem(spacer);

    // when checkbox is included
    if((input.count() >= 4) && (input.last().contains("checkbox")))
    {
        QCheckBox *checkbox = new QCheckBox("include",central);
        checkbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                            ,parameter,SLOT(checkBoxSlot(const int &)));
        widget_list->append(checkbox);
        inner_layout->addWidget(checkbox);
    }

    // adds parameter class to command list
    parameter->setWidgets(widget_list);
    command->addParameter(parameter);

    return inner_layout;
}

/*!
 * @brief Function to geretate parameter parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateCombobox(QList<QString> input, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widget_list = new QList<QObject*>();
    Parameter* parameter = new Parameter(input[TEXT_POS], " ");

    // layout for specified widgets
    QHBoxLayout *inner_layout = new QHBoxLayout();
    inner_layout->setSizeConstraint(QLayout::SetFixedSize);

    //
    QComboBox *combo = new QComboBox(central);
    combo->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    widget_list->append(combo);
    inner_layout->addWidget(combo);

    QList<QString> options = parseWidgetOptions(input[WIDGET_POS]);
    parameter->setValue(options[0] + DONT_INCLUDE);
    for(int i = 0; i < options.count(); i++)
    {
        combo->addItem(options[i]);
    }
    QObject::connect(combo,SIGNAL(currentTextChanged(const QString &))
                     ,parameter,SLOT(stringChangeSlot(const QString &)));

    QSpacerItem *spacer = new QSpacerItem(100,1, QSizePolicy::Fixed, QSizePolicy::Fixed);
    inner_layout->addSpacerItem(spacer);

    // when checkbox is included
    if((input.count() >= 4) && (input.last().contains("checkbox")))
    {
        QCheckBox *checkbox = new QCheckBox("include",central);
        checkbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                            ,parameter,SLOT(checkBoxSlot(const int &)));
        widget_list->append(checkbox);
        inner_layout->addWidget(checkbox);
    }

    // adds parameter class to command list
    parameter->setWidgets(widget_list);
    command->addParameter(parameter);

    return inner_layout;
}

/*!
 * @brief Function to geretate spinbox parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateSpinbox(QList<QString> input, QWidget *central)
{
    // list to hold widgets
    QList<QObject*>* widget_list = new QList<QObject*>();

    // layout for specified widgets
    QHBoxLayout *inner_layout = new QHBoxLayout();
    inner_layout->setSizeConstraint(QLayout::SetFixedSize);

    Parameter* parameter = new Parameter(input[TEXT_POS], QString("0%0").arg(DONT_INCLUDE));

    QSpinBox *spinbox = new QSpinBox(central);
    spinbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spinbox->setMaximum(1000);
    spinbox->setMinimum(-1000);
    QObject::connect(spinbox,SIGNAL(textChanged(const QString &))
                         ,parameter,SLOT(stringChangeSlot(const QString &)));
    widget_list->append(spinbox);
    inner_layout->addWidget(spinbox);

    QSpacerItem *spacer = new QSpacerItem(404, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
    inner_layout->addSpacerItem(spacer);

    // when checkbox is included
    if((input.count() >= 4) && (input.last().contains("checkbox")))
    {
        QCheckBox *checkbox = new QCheckBox("include",central);
        checkbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                            ,parameter,SLOT(checkBoxSlot(const int &)));
        widget_list->append(checkbox);
        inner_layout->addWidget(checkbox);
    }

    // adds parameter class to command list
    parameter->setWidgets(widget_list);
    command->addParameter(parameter);

    return inner_layout;
}

/*!
 * @brief Function to geretate mulitple spiboxes in one layout
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateMultiSpinbox(QList<QString> input, QWidget *central)
{
    // layout for specified widgets
    QHBoxLayout *inner_layout = new QHBoxLayout();
    inner_layout->setSizeConstraint(QLayout::SetFixedSize);

    // when checkbox is included
    QCheckBox* checkbox_pointer;
    if((input.count() >= 4) && (input.last().contains("checkbox")))
    {
        QCheckBox *checkbox = new QCheckBox("include",central);
        checkbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

        checkbox_pointer = checkbox;
    }

    int spinbox_number = parseWidgetNumber(input[WIDGET_POS]);
    for(int i = 0; i < spinbox_number; i++)
    {
        QList<QObject*>* widget_list = new QList<QObject*>();

        // first spinbox gets parameter's body
        QString text = "%0,";
        if(i == 0)
        {
            text = input[TEXT_POS];
        }
        // last one is without ","
        else if(i == spinbox_number -1)
        {
            text = "%0";
        }

        Parameter* parameter = new Parameter(text, QString("0%0").arg(DONT_INCLUDE));

        QSpinBox *spinbox = new QSpinBox(central);
        spinbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        spinbox->setMaximum(1000);
        spinbox->setMinimum(-1000);
        QObject::connect(spinbox,SIGNAL(textChanged(const QString &))
                             ,parameter,SLOT(stringChangeSlot(const QString &)));
        widget_list->append(spinbox);
        inner_layout->addWidget(spinbox);


        // connecitng checkbox to every spinbox
        QObject::connect(checkbox_pointer,SIGNAL(stateChanged(const int &))
                            ,parameter,SLOT(checkBoxSlot(const int &)));
        widget_list->append(checkbox_pointer);

        // adds parameter class to command list
        parameter->setWidgets(widget_list);
        parameter->setWidgetType(WidgetType::MULTISPINBOX);

        command->addParameter(parameter);
    }

    // spacer for astetics
    QSpacerItem *spacer = new QSpacerItem(390 -( 60 * (spinbox_number-1)), 1, QSizePolicy::Minimum, QSizePolicy::Minimum);
    inner_layout->addSpacerItem(spacer);

    inner_layout->addWidget(checkbox_pointer);

    return inner_layout;
}

/*!
 * @brief Function to geretate float spinbox parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateDoubleSpinbox(QList<QString> input, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widget_list = new QList<QObject*>();
    Parameter* parameter = new Parameter(input[TEXT_POS], QString("0,0%0").arg(DONT_INCLUDE));

    // layout for specified widgets
    QHBoxLayout *inner_layout = new QHBoxLayout();
    inner_layout->setSizeConstraint(QLayout::SetFixedSize);

    //
    QDoubleSpinBox *spinbox = new QDoubleSpinBox(central);
    spinbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spinbox->setMaximum(1000);
    spinbox->setMinimum(-1000);
    spinbox->setDecimals(1);
    QObject::connect(spinbox,SIGNAL(textChanged(const QString &))
                         ,parameter,SLOT(stringChangeSlot(const QString &)));

    widget_list->append(spinbox);
    inner_layout->addWidget(spinbox);

    QSpacerItem *spacer = new QSpacerItem(390, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
    inner_layout->addSpacerItem(spacer);

    // when checkbox is included
    if((input.count() >= 4) && (input.last().contains("checkbox")))
    {
        QCheckBox *checkbox = new QCheckBox("include",central);
        checkbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                            ,parameter,SLOT(checkBoxSlot(const int &)));
        widget_list->append(checkbox);
        inner_layout->addWidget(checkbox);
    }

    // adds parameter class to command list
    parameter->setWidgets(widget_list);
    command->addParameter(parameter);

    return inner_layout;
}

/*!
 * @brief Function to geretate file selection parameter
 * @param QList<QString> settings - &p|...|| splited by "|"
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateFileSearch(QList<QString> input, QWidget *central)
{
    // list of created widgets
    QList<QObject*>* widget_list = new QList<QObject*>();
    Parameter* parameter = new Parameter(input[TEXT_POS], DONT_INCLUDE);

    // layout for specified widgets
    QHBoxLayout *inner_layout = new QHBoxLayout();
    inner_layout->setSizeConstraint(QLayout::SetFixedSize);


    QLineEdit *line_edit = new QLineEdit(central);
    line_edit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    QObject::connect(line_edit,SIGNAL(textChanged(const QString &))
                     ,parameter,SLOT(stringChangeSlot(const QString &)));
    widget_list->append(line_edit);
    inner_layout->addWidget(line_edit);

    QPushButton *button = new QPushButton("...", central);
    button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QObject::connect(button,&QPushButton::pressed
                     ,parameter,&Parameter::fileSelectSlot);
    widget_list->append(button);
    inner_layout->addWidget(button);


    // when checkbox is included
    if((input.count() >= 4) && (input.last().contains("checkbox")))
    {
        QCheckBox *checkbox = new QCheckBox("include",central);
        checkbox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QObject::connect(checkbox,SIGNAL(stateChanged(const int &))
                            ,parameter,SLOT(checkBoxSlot(const int &)));
        widget_list->append(checkbox);
        inner_layout->addWidget(checkbox);
    }

    // adds parameter class to command list
    parameter->setWidgets(widget_list);
    parameter->setMainWindow(main_window);
    command->addParameter(parameter);

    return inner_layout;
}


/*!
 * @brief If command has no parameters this is used to inform about it
 * @param QWidget* central - pointer to parent widget
 * @return QLayout*
 */
QLayout *CommandOptionsHandler::generateNoParametersLabel(QWidget *central)
{
    QVBoxLayout* outer_layout = new QVBoxLayout();
    outer_layout->setSizeConstraint(QLayout::SetDefaultConstraint);

    // informative label
    QLabel *label = new QLabel(NO_PARAM_MESSAGE,central);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    outer_layout->addWidget(label);

    return outer_layout;
}

/*!
 * @brief  return command name from file line, object adds the rest to the list
 * @param  QString line - line from "commands.txt"
 * @return QString - command name
 */
QString CommandOptionsHandler::parseFileLine(QString line)
{
    QString command_name;
    QString command_generator;

    // add command parameters to list
    command_generator = parseCommandGenerator(line);
    list_settings << command_generator;

    // get command name and remove "markers"
    command_name = parseCommandName(line);

    return command_name;
}
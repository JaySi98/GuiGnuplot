#include "lineParsing.h"

QString parseCommandGenerator(QString line)
{
    QString command;
    int param_start_pos = line.indexOf(PARAM_SYMBOL);

    command = line.mid(param_start_pos);

    return command;
}

QString parseCommandName(QString line)
{
    QString command_name;
    int name_start_pos = line.indexOf(NAME_SYMBOL);
    int name_end_pos = line.indexOf(END_SYMBOL);
    int length = name_end_pos - name_start_pos;

    command_name = line.mid(name_start_pos, length);
    command_name.remove(0,SYMBOL_LENGTH);

    return command_name;
}

QString parseCommandSettings(QString line)
{
    QString command_parameters;
    int param_start_pos = line.indexOf("&p");

    command_parameters = line.mid(param_start_pos);

    return command_parameters;
}

QString parseSingleSetting(QString line)
{
    QString output;
    int setting_start_pos = line.indexOf("|");
    int setting_end_pos = line.indexOf("||",setting_start_pos);
    int length = setting_end_pos - setting_start_pos;
    // removes outer "|" signs
    output = line.mid(setting_start_pos+1,length-1);
    return output;
}

QList<QString> parseWidgetOptions(QString input)
{
    int first_bracket_pos = input.indexOf("(");
    int second_bracket_pos = input.indexOf(")");
    int length = second_bracket_pos - first_bracket_pos;

    QString trimmed = input.mid(first_bracket_pos+1,length-1);

    QList<QString> output = trimmed.split(",");

    return output;
}

int parseWidgetNumber(QString input)
{
    int first_bracket_pos = input.indexOf("(");
    int second_bracket_pos = input.indexOf(")");
    int length = second_bracket_pos - first_bracket_pos;

    QString trimmed = input.mid(first_bracket_pos+1,length-1);

    int output = trimmed.toInt();

    return output;
}

QList<QString> splitSettings(QString line)
{
    QList<QString> temp;
    QList<QString> list_settings;

    QString settings = parseCommandSettings(line);

    temp = settings.split("&p");
    for(int i = 0; i < temp.count(); i++)
    {
        QString current_setting = temp[i];
        if(current_setting.contains(END_SYMBOL))
        {
            current_setting = parseSingleSetting(current_setting);

            list_settings << current_setting;
        }
    }

    return list_settings;
}

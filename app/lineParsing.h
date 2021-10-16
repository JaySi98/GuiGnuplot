#ifndef LINEPARSING_H
#define LINEPARSING_H

#include <QString>
#include <QList>

#define MIN_SETTING_NUM 2
#define SYMBOL_LENGTH   3
#define TEXT_POS        0
#define LABEL_POS       1
#define WIDGET_POS      2
#define CHECK_POS       3

#define PARAM_SYMBOL     "&p|"
#define NAME_SYMBOL      "&n|"
#define END_SYMBOL       "||"
#define DONT_INCLUDE     "dont include"
#define NO_PARAM_MESSAGE "This command doesn't have parameters"

QString        parseCommandGenerator(QString);
QString        parseCommandName(QString line);
QString        parseCommandSettings(QString line);
QString        parseSingleSetting(QString);
QList<QString> parseWidgetOptions(QString input);
int            parseWidgetNumber(QString input);
QList<QString> splitSettings(QString);

#endif // LINEPARSING_H

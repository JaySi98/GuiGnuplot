#ifndef ENUMS_H
#define ENUMS_H

/*
 * Header file to contain enumeration types
 */

// tutaj sensowne komentarze
enum class OutputType  {STANDARD, ERROR, SUCCESS, IGNORE};

//enum class SettingType {NO_PARAMETERS, LINE_EDIT, FILE_SELECT, SPINBOX, MULTISPINBOX, DOUBLE_SPINBOX, COMBOBOX, CHECKBOX};

enum class WidgetType  { LINE_EDIT, COMBOBOX, SPINBOX, MULTISPINBOX, DOUBLE_SPINBOX, FILE_SEARCH, NO_PARAMS, INVALID};

#endif // ENUMS_H

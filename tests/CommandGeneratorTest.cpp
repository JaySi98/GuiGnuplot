#include <QApplication>
#include "catch.hpp"
#include "commandGenerator.h"
#include "command.h"
#include "parameter.h"

TEST_CASE("Testing CommandOptionsHandler class")
{
    CommandOptionsHandler* testCommandHandler = new CommandOptionsHandler();

    SECTION("createNewCommand test")
    {
       testCommandHandler->createNewCommand("name");

       REQUIRE(testCommandHandler->getCommandString() == "name");
    }

    SECTION("resolveParameter test")
    {
        QString input = "text";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::LINE_EDIT);
        input = "file selection";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::FILE_SEARCH);
        input = "selection";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::COMBOBOX);
        input = "float numeric";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::DOUBLE_SPINBOX);
        input = "multiple numerics";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::MULTISPINBOX);
        input = "numeric";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::SPINBOX);
        input = "no parameters";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::NO_PARAMS);
        input = "incorrect";
        REQUIRE(testCommandHandler->resolveParameter(input) ==  WidgetType::INVALID);
    }

    delete testCommandHandler;
}

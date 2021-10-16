#include "catch.hpp"
#include "command.h"
#include "parameter.h"

TEST_CASE("Testing Command class")
{
    Command* testCommand = new Command();

    SECTION("setName test")
    {
        testCommand->setName("commandName");

        REQUIRE(testCommand->getName() == "commandName");
    }

    SECTION("addParameter test")
    {
        Parameter* param1 = new Parameter("body1 %0","value1");
        Parameter* param2 = new Parameter("body2 %0","value2");
        Parameter* param3 = new Parameter("body3 %0","value3");

        testCommand->addParameter(param1);
        testCommand->addParameter(param2);
        testCommand->addParameter(param3);

        REQUIRE(testCommand->getParameters().count() == 3);
        REQUIRE(testCommand->getParameters()[0]->toQString() == "body1 value1");
        REQUIRE(testCommand->getParameters()[1]->toQString() == "body2 value2");
        REQUIRE(testCommand->getParameters()[2]->toQString() == "body3 value3");
    }

    SECTION("freeMemory test")
    {
        testCommand->freeMemory();

        REQUIRE(testCommand->getParameters().count() == 0);
    }

    delete testCommand;

    Command* testCommand2 = new Command();

    SECTION("toQstring test1")
    {
        testCommand2->setName("test");

        REQUIRE(testCommand2->toQString() == "test");
    }

    SECTION("toQstring test2")
    {
        testCommand2->setName("test");
        Parameter* param1 = new Parameter("body %0", "value");
        testCommand2->addParameter(param1);

        REQUIRE( testCommand2->toQString() == "test body value");
    }

    delete testCommand2;
}

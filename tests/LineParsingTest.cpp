#include "catch.hpp"
#include "lineParsing.h"

TEST_CASE("Testing line parsing")
{
    SECTION("parseCommandGenerator test")
    {
        QString testLine = "&n|set mztics|| &p|%0|value|text|checkbox||";
        QString output = parseCommandGenerator(testLine);

        REQUIRE(output == "&p|%0|value|text|checkbox||");
    }

    SECTION("parseCommandName test")
    {
        QString testLine = "&n|set mztics|| &p|%0|value|text|checkbox||";
        QString output = parseCommandName(testLine);

        REQUIRE(output == "set mztics");
    }

    SECTION("parseCommandSettings test")
    {
        QString testLine = "&n|set mztics|| &p|%0|value|text|checkbox||";
        QString output = parseCommandSettings(testLine);

        REQUIRE(output == "&p|%0|value|text|checkbox||");
    }

    SECTION("parseSingleSetting test")
    {
        QString testLine = "&p|%0|value|text|checkbox||";
        QString output = parseSingleSetting(testLine);

        REQUIRE(output == "%0|value|text|checkbox");
    }

    SECTION("parseWidgetOptions test")
    {
        QString testLine = "&p|%0|axes|selection(x,y,z,cb,x2,y2,xy)|checkbox||";
        QList<QString> output = parseWidgetOptions(testLine);

        REQUIRE(output.count() == 7);
        REQUIRE(output[0] == "x");
        REQUIRE(output[6] == "xy");
    }

    SECTION("parseWidgetNumber test")
    {
        QString testLine = "&p|%0,|sample 1, sample 2|multiple numerics(2)|checkbox||";
        int output = parseWidgetNumber(testLine);

        REQUIRE(output == 2);
    }

    SECTION("splitSettings test")
    {
        QString testLine = "&p|test1|| &p|test2|| &p|test3|| &p|test4||";
        QList<QString> output = splitSettings(testLine);

        REQUIRE(output.count() == 4);
        REQUIRE(output[0] == "test1");
        REQUIRE(output[3] == "test4");
    }
}

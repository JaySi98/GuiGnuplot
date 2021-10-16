#include <QApplication>
#include "catch.hpp"
#include "parameter.h"

TEST_CASE("Testing Parameter class")
{
    int argc; char *argv[1];
    QApplication a(argc, argv);

    Parameter* testParameter = new Parameter("body", "value");

    SECTION("constructor test")
    {
        REQUIRE(testParameter->getBody() == "body");
        REQUIRE(testParameter->getValue() == "value");
    }

    SECTION("toQstring test1")
    {
        testParameter->setBody("testbody %0");
        testParameter->setValue("testvalue");

        REQUIRE(testParameter->toQString() == "testbody testvalue");
    }

    SECTION("toQstring test2")
    {
        testParameter->setBody("testbody");
        testParameter->setValue("testvalue");

        REQUIRE(testParameter->toQString() == "testbody");
    }

    SECTION("toQstring test3")
    {
        testParameter->setBody("testbody %0");
        testParameter->setValue(DONT_INCLUDE);

        REQUIRE(testParameter->toQString() == nullptr);
    }

    SECTION("Adding widget list test")
    {
        QList<QObject*>* widget_list = new QList<QObject*>();
        testParameter->setWidgets(widget_list);

        REQUIRE(testParameter->getWidgets() != nullptr);
    }

    SECTION("stringChangeSlot test2")
    {
        testParameter->setValue(DONT_INCLUDE);
        testParameter->stringChangeSlot("value");

        REQUIRE(testParameter->getValue() == "valuedont include");
    }

    SECTION("stringChangeSlot test2")
    {
        testParameter->setValue("value");
        testParameter->stringChangeSlot("different value");

        REQUIRE(testParameter->getValue() == "different value");
    }

    SECTION("checkBoxSlot test")
    {
        testParameter->setValue("value");
        testParameter->checkBoxSlot(0);

        REQUIRE(testParameter->getValue() == "valuedont include");

        testParameter->checkBoxSlot(2);

        REQUIRE(testParameter->getValue() == "value");
    }

    delete testParameter;
}

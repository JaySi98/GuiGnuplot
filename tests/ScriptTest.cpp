#include <QApplication>
#include "catch.hpp"
#include "script.h"

TEST_CASE("Testing Script class")
{
    int argc; char *argv[1];
    QApplication a(argc, argv);
    QMainWindow window;

    SECTION("empty constructor test")
    {

        Script* testScript = new Script(&window);

        REQUIRE(testScript->getFileName() == " ");
        REQUIRE(testScript->getText() == "");
        delete testScript;
    }

    SECTION("constructor with parameters test")
    {
        Script* testScript = new Script("name","inside", &window);

        REQUIRE(testScript->getFileName() == "name");
        REQUIRE(testScript->getText() == "inside");
        delete testScript;
    }

    Script* testScript = new Script("name","inside",&window);

    SECTION("setText and getText test")
    {
        testScript->setText("test");

        REQUIRE(testScript->getText() == "test");
    }

    SECTION("isSaved test1")
    {
        testScript->setText("test");

        REQUIRE(testScript->isSaved() == false);
    }

    SECTION("isSaved test2")
    {
        testScript->setText("test");
        testScript->setPreviousText("test");

        REQUIRE(testScript->isSaved() == true);
    }

    delete testScript;
}

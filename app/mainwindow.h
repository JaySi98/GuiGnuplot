/*****************************************************
 * FILE NAME                                         *
 * mainwindow.h                                      *
 *                                                   *
 * PURPOSE                                           *
 * This is the main Window of the application.       *
 * Everything realted to GUI is here.                *
 *****************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QShortcut>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QToolButton>
#include <QTextEdit>
//
#include "gnuplotProcess.h"
#include "highlighter.h"
#include "enums.h"
#include "commandGenerator.h"
#include "script.h"

#define MAX_SCRIPTS_COUNT 12
#define WINDOW_WIDTH 1310
#define WINDOW_HEIGHT 775


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void disableCompilation();
    void output(QString data, OutputType type);

private slots:
    void searchCommand();
    void scriptChanged();
    void closeCurrentScript();
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void compile();
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void about();


private:
    void closeEvent(QCloseEvent *event);
    void printOS();
    bool isScriptAlreadyOpened(QString fileName);

    // dynamic GUI for Gnuplot commands
    void showHelp();
    void addCommandToScript();
    void loadCommand();
    void parseCommandList();
    void setCommandWidgets();
    void clearCommandWidgets();

    Ui::MainWindow*        ui;
    GnuplotProcess*        gnuplotHelp;
    CommandOptionsHandler* command_handler;

    QList<QString>    command_list;         // list of supported gnuplot commands
    QListWidgetItem*  selected_command;     // currently selected command
    int               selected_command_num; // number of currently selected command

    QList<Script*>    scripts;
    Script*           currentScript;
};
#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->centralwidget);

    // first script
    currentScript = new Script();
    scripts.append(currentScript);
    QTextEdit* textEdit = currentScript->getTextEdit();
    QVBoxLayout* tabLayout = new QVBoxLayout(this);
    tabLayout->addWidget(textEdit);
    ui->tabWidget->currentWidget()->setLayout(tabLayout);

    gnuplot_process = new GnuplotProcess(this);
    command_handler = new CommandOptionsHandler(this);

    // anything emited by other classes will be displayed
    // on plain_text_oputput widget
    connect(gnuplot_process,&GnuplotProcess::output, this,&MainWindow::output);
    connect(gnuplot_process,&GnuplotProcess::gnuplotNotDetected, this,&MainWindow::disableCompilation);
    connect(command_handler,&CommandOptionsHandler::output,this,&MainWindow::output);

    // print detected operating system
    printOS();
    // open process in the background
    gnuplot_process->start();

    // loading all gnuplot commands into listWidget_commands
    parseCommandList();

    // for text editor functionality
    connect(ui->actionNew,     &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen,    &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave,    &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(ui->actionCompile, &QAction::triggered, this, &MainWindow::compile);
    connect(ui->actionCopy,    &QAction::triggered, this, &MainWindow::copy);
    connect(ui->actionPaste,   &QAction::triggered, this, &MainWindow::paste);
    connect(ui->actionCut,     &QAction::triggered, this, &MainWindow::cut);
    connect(ui->actionUndo,    &QAction::triggered, this, &MainWindow::undo);
    connect(ui->actionRedo,    &QAction::triggered, this, &MainWindow::redo);
    connect(ui->actionClose,   &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout,   &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionCloseCurrentScript, &QAction::triggered, this, &MainWindow::closeCurrentScript);
    connect(ui->lineEdit_search,        SIGNAL(textChanged(const QString &)),this,SLOT(searchCommand()));
    connect(ui->pushButton_add_command, &QPushButton::clicked, this, &MainWindow::addCommandToScript);
    connect(ui->listWidget_commands,    &QListWidget::itemSelectionChanged, this, &MainWindow::loadCommand);
    connect(ui->pushButton_help,        &QPushButton::clicked, this, &MainWindow::showHelp);
    connect(ui->tabWidget,              &QTabWidget::currentChanged, this, &MainWindow::scriptChanged);

    // key shortcuts
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this, SLOT(newFile()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_G), this, SLOT(compile()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(saveFile()));

    setWindowIcon(QIcon(":/resources/gnuplotIcon.ico"));
}

MainWindow::~MainWindow()
{
    for(int i = 0; i < scripts.count(); i++)
    {
        delete scripts.at(i);
    }
    delete gnuplot_process;
    delete command_handler;
    delete ui;
}


bool MainWindow::isScriptAlreadyOpened(QString fileName)
{
    for(int i = 0; i < scripts.count(); i++)
    {
        if(scripts[i]->getFileName().compare(fileName) == 0)
        {
            return true;
        }
    }
    return false;
}

/*!
 * @brief Displays info onto "plain_text_output" widget
 */

void MainWindow::output(QString data, OutputType type)
{
    if(type == OutputType::STANDARD)
        //black
        ui->plain_text_output->setTextColor(QColor(0,0,0));
    else if(type == OutputType::ERROR)
        //red
        ui->plain_text_output->setTextColor(QColor(255,0,0));
    else if(type == OutputType::SUCCESS)
        //green
        ui->plain_text_output->setTextColor(QColor(0,255,0));

    ui->plain_text_output->append(data);
}

/*!
 * @brief Prints OS detected by gnuplot_process
 */
void MainWindow::printOS()
{
    QString system = gnuplot_process->getOperatingSystem();
    ui->plain_text_output->append("Deteced operating system: "
                                            + system);
}

/*!
 * @brief Closing program sequence
 */
void MainWindow::closeEvent (QCloseEvent *event)
{
    int unsavedScripts = 0;
    for(int i = 0; i < scripts.count();i++)
    {
        if(!scripts[i]->isSaved())
        {
            unsavedScripts++;
        }
    }

    if(unsavedScripts > 0)
    {
        QMessageBox::StandardButton result;

        if(unsavedScripts == 1)
        {
            result = QMessageBox::question(this, "Unsaved changes",
                                         QString("There is one unsaved Script. Do you want to exit"
                                         " without saving it?").arg(unsavedScripts),
                                         QMessageBox::Yes| QMessageBox::Cancel);
        }
        else
        {
            result = QMessageBox::question(this, "Unsaved changes",
                                         QString("There are %0 unsaved scripts. Do you want to exit"
                                         " without saving them?").arg(unsavedScripts),
                                         QMessageBox::Yes| QMessageBox::Cancel);
        }

        if(result == QMessageBox::Yes)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->accept();
    }
}

/* ******************** DYNAMIC GUI FOR GNUPLOT COMMANDS ******************* */

/*!
 * @brief Loads commands from .txt file into a list and QListWidget
 */
void MainWindow::parseCommandList()
{
    QString fileName = ":commands/resources/commands.txt";

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QString command_name;

          // commandHandler returns command name and holds the rest in list
          command_name = command_handler->parseFileLine(line);

          command_list << command_name;

          ui->listWidget_commands->addItem(command_name);
       }
       inputFile.close();
    }
    else
    {
        output("Could not open 'commands.txt'",OutputType::ERROR);
    }
}

/*!
 * @brief Searches list widget for specified command
 */
void MainWindow::searchCommand()
{
    QString input = ui->lineEdit_search->text();
    int list_count = ui->listWidget_commands->count();

    for( int i = 0; i< list_count; i++)
    {
        QListWidgetItem* command = ui->listWidget_commands->item(i);

        // hide commands that dont match searching phrase
        if(command->text().contains(input,Qt::CaseInsensitive))
        {
            command->setHidden(false);
        }
        else
            command->setHidden(true);
    }
}

/*!
 * @brief delets widgets for current command and set the new one
 */
void MainWindow::loadCommand()
{
    selected_command = ui->listWidget_commands->currentItem();
    selected_command_num = ui->listWidget_commands->currentRow();

    // at first "help" and "add command" buttons are disabled
    if(!ui->pushButton_help->isEnabled())
        ui->pushButton_help->setEnabled(true);

    if(!ui->pushButton_add_command->isEnabled())
        ui->pushButton_add_command->setEnabled(true);

    //
    clearCommandWidgets();
    setCommandWidgets();
}

/*!
 * @brief sets widgets for currently selected gnuplot command
 */
void MainWindow::setCommandWidgets()
{
    QScrollArea *scroll_area = qobject_cast<QScrollArea*>(ui->scrollArea_params);

    QWidget *central = command_handler->getCommandWidgets(selected_command->text(),selected_command_num);
//    QWidget *central = command_handler->GetGridLayout(selected_command->text(),selected_command_num);

    scroll_area->setWidget(central);
}

/*!
 * @brief deletes widgets inside scrollArea_options
 */
void MainWindow::clearCommandWidgets()
{
    // every child widget of central is also destroyed
    QWidget *central = ui->scrollArea_params->widget();

    if(central)
    {
        delete central;

        // deletes Command and Parameter objects
        command_handler->freeMemory();
    }
}

/*!
 * @brief adds selected command with parameters to the script
 */
void MainWindow::addCommandToScript()
{
    QString command = command_handler->getCommandString();

    currentScript->addCommand(command);
}

/*!
 * @brief Shows gnuplot help window for selected command
 */
void MainWindow::showHelp()
{
    output("\nShowing help for \"" + selected_command->text()
           + "\" command", OutputType::STANDARD);

    gnuplot_process->help(selected_command->text());
}

/* *********************** SLOTS *********************** */
void MainWindow::scriptChanged()
{
    int index = ui->tabWidget->currentIndex();
    currentScript = scripts[index];
}

void MainWindow::disableCompilation()
{
    ui->actionCompile->setDisabled(true);
}

void MainWindow::closeCurrentScript()
{
    QMessageBox::StandardButton result = QMessageBox::No;

    if(!currentScript->isSaved())
    {
        result = QMessageBox::question(this, "Save",
                  "Do you want to save current script?",
                  QMessageBox::No | QMessageBox::Yes
                  | QMessageBox::Cancel);
    }

    if(result != QMessageBox::Cancel)
    {
        if(result == QMessageBox::Yes)
        {
            saveFile();
        }

        if(scripts.count() > 1)
        {
            int scriptIndex = ui->tabWidget->currentIndex();
            Script* toClose = scripts[scriptIndex];

            ui->tabWidget->removeTab(scriptIndex);
            scripts.removeAt(scriptIndex);
            delete toClose;

            for(int i = 0; i < scripts.count(); i++)
            {
                if(scripts[i] != nullptr)
                {
                    currentScript = scripts[i];
                    ui->tabWidget->setCurrentIndex(i);
                }
            }
        }
        else
        {
            currentScript->setFileName("");
            currentScript->setPreviousText("");
            currentScript->setText("");
        }

        // enable creating new scripts
        if(!ui->actionNew->isEnabled())
        {
            ui->actionNew->setEnabled(true);
            ui->actionOpen->setEnabled(true);
        }
    }
}

void MainWindow::newFile()
{
    int scriptsCount = scripts.count();

    disconnect(currentScript->getTextEdit(), nullptr, nullptr, nullptr);
    currentScript = new Script();
    scripts.append(currentScript);

    QTextEdit* textEdit = currentScript->getTextEdit();
    QVBoxLayout* tabLayout = new QVBoxLayout(this);
    tabLayout->addWidget(textEdit);

    QWidget* page = new QWidget();
    ui->tabWidget->addTab(page,QString("script %0").arg(scriptsCount+1));
    ui->tabWidget->setCurrentWidget(page);
    ui->tabWidget->currentWidget()->setLayout(tabLayout);

    output("\nCreated new script", OutputType::STANDARD);

    if(scriptsCount+1 >= MAX_SCRIPTS_COUNT)
    {
        ui->actionNew->setEnabled(false);
        ui->actionOpen->setEnabled(false);
    }
}

void MainWindow::openFile()
{
    int scriptsCount = scripts.count();

    QString file_open = QFileDialog::getOpenFileName(this, "Select file","",
                                                    tr("Text files (*.txt)"));
    if(!isScriptAlreadyOpened(file_open) && !file_open.isNull())
    {
        QFile file_stream(file_open);
        if(file_stream.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream input(&file_stream);
            QString text = input.readAll();
            file_stream.close();

            disconnect(currentScript->getTextEdit(), nullptr, nullptr, nullptr);
            currentScript = new Script(file_open, text);
            scripts.append(currentScript);

            QTextEdit* textEdit = currentScript->getTextEdit();
            QVBoxLayout* tabLayout = new QVBoxLayout(this);
            tabLayout->addWidget(textEdit);

            QWidget* page = new QWidget();
            ui->tabWidget->addTab(page,QString("script %0").arg(scriptsCount+1));
            ui->tabWidget->setCurrentWidget(page);
            ui->tabWidget->currentWidget()->setLayout(tabLayout);

            output("\nopened " + currentScript->getFileName(), OutputType::STANDARD);
         }
         else
         {
             QMessageBox::warning(this, "Warning","Cannot open file: "
                                  + file_stream.errorString());
         }
    }
    else if(!file_open.isNull())
    {
        QMessageBox msgBox;
        msgBox.setText("This file is already opened!");
        msgBox.exec();
    }

    if(scriptsCount+1 >= MAX_SCRIPTS_COUNT)
    {
        ui->actionNew->setEnabled(false);
        ui->actionOpen->setEnabled(false);
    }
}

void MainWindow::saveFile()
{
    if(currentScript->getFileName().isEmpty())
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Save file","../",
                                                        tr("Text files (*.txt)"));
        currentScript->setFileName(fileName);
    }

    QFile file_stream(currentScript->getFileName());
    if(file_stream.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream text_stream(&file_stream);

        QString text = currentScript->getText();
        text_stream << text;

        currentScript->setPreviousText(text);

        file_stream.flush();
        file_stream.close();

        output("\nSaved " + currentScript->getFileName(), OutputType::STANDARD);
    }
}

void MainWindow::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save file","../",
                                                    tr("Text files (*.txt)"));
    if(currentScript->getFileName().isEmpty())
    {
        currentScript->setFileName(fileName);
    }

    QFile file_stream(fileName);
    if(file_stream.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream text_stream(&file_stream);

        QString text = currentScript->getText();
        text_stream << text;

        currentScript->setPreviousText(text);

        file_stream.flush();
        file_stream.close();

        output("\nSaved as " + fileName, OutputType::STANDARD);
    }
}

/*!
 * @brief creates graph from based on what is on the canvas
 */
void MainWindow::compile()
{
    QString script = currentScript->getText();

    if(!script.isEmpty())
    {
        output("\nGenerating graph", OutputType::STANDARD);

        gnuplot_process->setScript(script);
        gnuplot_process->generateGraph();
    }
    else
    {
        QMessageBox::warning(this, "Empty script",
                              "Nothing to compile",
                              QMessageBox::Ok);
    }
}

void MainWindow::copy()
{
    currentScript->copy();
}


void MainWindow::cut()
{
   currentScript->cut();
}

void MainWindow::paste()
{
    currentScript->paste();
}

void MainWindow::undo()
{
    currentScript->undo();
}

void MainWindow::redo()
{
    currentScript->redo();

    qDebug() << this->width();
}

/*!
 * @brief Displays credits
 * and basic information about app
 */
void MainWindow::about()
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("About");
    msgBox.setText("The application is an engineering thesis written by Jakub Szkiładź "
                   "under the supervision of PhD Eng. Krzysztof Jurczuk at "
                   "the Faculty of Computer Science of the Białystok University of Technology.\n\n"
                   "The application is a graphical interface designed to facilitate "
                   "work with the gnuplot program. ");
    msgBox.exec();
}

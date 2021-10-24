#include "gnuplotProcess.h"

GnuplotProcess::GnuplotProcess(QObject *parent) : QObject(parent)
{
    // connecting slots with QProcess process
    connect(&process, &QProcess::errorOccurred,
            this, &GnuplotProcess::errorOccurred);
    connect(&process, &QProcess::readyReadStandardOutput,
            this, &GnuplotProcess::readyReadStandardOutput);
    connect(&process, &QProcess::started,
            this, &GnuplotProcess::started);
    connect(&process,QOverload<int,
            QProcess::ExitStatus>::of(&QProcess::finished),
            this,&GnuplotProcess::finished);
    connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,&GnuplotProcess::finished);
    connect(&process, &QProcess::stateChanged,
            this, &GnuplotProcess::stateChanged);

    // detecting operating system
    operating_system = QSysInfo::productType();
}


GnuplotProcess::~GnuplotProcess()
{
    stop();
}

/*!
 * @brief start the process
 */

void GnuplotProcess::start()
{
    qInfo() << Q_FUNC_INFO;
    // open console
    process.start(getProcess(), QStringList());
    // open gnuplot
    writeCommand("gnuplot 2>&1");

    readyReadStandardOutput();
}

/*!
 * @brief
 * Sends every line of script as command to opened process.
 * If script is correct, gnuplot window should open with
 * expected graph. Otherwise an error will be printed.
 */

void GnuplotProcess::generateGraph()
{

    QByteArray command;
    QString line;
    QTextStream stream(&script);

    addCommandsToScript();

    while (stream.readLineInto(&line))
    {        
        writeCommand(line);
    }
}

/*!
 * @brief adds addictional commands to the script
 */

void GnuplotProcess::addCommandsToScript()
{
    // to reset all settings
    script.insert(0, "reset \n");
    // to reset previous errors if any occured
    script.insert(0, "reset errors \n");
    // I dont know why but it's necessary
    script.insert(0, "# \n");
    // to inform whether operation succeded or not
    script.append("\n if(GPVAL_ERRNO == 0) "
                        "{print \"Done\"} "
                      "else "
                        "{print \"Error occured\"} \n");
}

/*!
 * @brief sends given command to process.
 */
void GnuplotProcess::writeCommand(QString input)
{
    QByteArray command = input.toUtf8();

    if(operating_system == "windows") command.append("\r");
    command.append("\n");

    process.write(command);
}

/*!
 * @brief sends  "help + command" to gnuplot
 * to display help window
 */
void GnuplotProcess::help(QString input)
{
    QString command = " help " + input + "\n";
    writeCommand(command);
}

/*!
 * @brief Based on what the OS returns what to open
 */

QString GnuplotProcess::getProcess()
{
    qInfo() << Q_FUNC_INFO;

    if(operating_system == "windows") return "cmd";
    if(operating_system == "osx") return "/bin/zsh";
    return "bash";
}

/*!
 * @brief Emits everything returned by Gnuplot
 */

void GnuplotProcess::readyReadStandardOutput()
{
    qInfo() << Q_FUNC_INFO;
    QByteArray data = process.readAllStandardOutput().trimmed();
    qInfo() << data;

    OutputType type = checkStandardOutput(data);

    if(type != OutputType::IGNORE)
        emit output(data, type);
}

/*!
 * @brief checks what type of output was recived
 */
OutputType GnuplotProcess::checkStandardOutput(QByteArray input)
{
    QString input_string = QString(input);
    QString meaningless[2] = {"gnuplot 2>&1","Microsoft Windows"};
    QString phrases[6] = {"gnuplot>", "undefined variable",
                          "invalid command", "Error occured",
                          "Cannot find or open file", "No data in plot"};
    QString noGnuplotPhrase = "'gnuplot' is not recognized as an internal or external command,"
                              "\r\noperable program or batch file.";

    if(input_string.contains(noGnuplotPhrase))
    {
        noGnuplot = true;
        gnuplotNotDetected();
    }

    if(noGnuplot)
    {
        emit output("Application could not detect gnuplot program.", OutputType::ERROR);
        emit output("Try to reinstall gnuplot or add it's path to system environment variables.",
                    OutputType::STANDARD);
        return OutputType::IGNORE;
    }
    else
    {
        for(int i = 0; i < int(sizeof(meaningless)/sizeof(*meaningless)); i++)
        {
            if((input_string.contains(meaningless[i])))
            {
                return OutputType::IGNORE;
            }
        }

        for(int i = 0; i < int(sizeof(phrases)/sizeof(*phrases)); i++)
        {
            if(input_string.contains(phrases[i]))
            {
                return OutputType::ERROR;
            }
        }

        if(input_string.contains("Done"))
            return OutputType::SUCCESS;

        return OutputType::STANDARD;
    }
}

/*!
 * @brief Informs that state of the process has changed
 */

void GnuplotProcess::stateChanged(QProcess::ProcessState new_state)
{
    qInfo() << Q_FUNC_INFO;

    switch (new_state)
    {

        case QProcess::Starting:
        {
            if(noGnuplot == false)
            {
                emit output("Starting Gnuplot process", OutputType::STANDARD);
                break;
            }
         }

        case QProcess::NotRunning:
            emit output("Closing Gnuplot process", OutputType::STANDARD);
            break;

        case QProcess::Running:
        {
            if(noGnuplot == false)
            {
                emit output("Gnuplot is running", OutputType::STANDARD);
                break;
            }
        }
    }
}

/* ************************** GETTERS AND SETTERS ************************** */

QString GnuplotProcess::getOperatingSystem()
{
    return this->operating_system;
}


void GnuplotProcess::setScript(QString script)
{
    this->script = script;
}


QString GnuplotProcess::getScript()
{
    return this->script;
}

/* **************************** LESS IMPORTANT ***************************** */
/*!
 * @brief Emitts the error if either the console or Gnuplot
 * couldn't be open
 */

void GnuplotProcess::errorOccurred(QProcess::ProcessError error)
{
    qInfo() << Q_FUNC_INFO << error;
}

/*!
 * @brief Usless function but might be used later.
 * It is only called when process is closed
 * so when the whole application is closed
 */

void GnuplotProcess::finished(int exitcode, QProcess::ExitStatus exit_status)
{
    qInfo() << Q_FUNC_INFO;
    Q_UNUSED(exitcode);
    Q_UNUSED(exit_status);
}

/*!
 * @brief closes the process
 */

void GnuplotProcess::stop()
{
    qInfo() << Q_FUNC_INFO;

    process.close();
}

/*!
 * @brief Informs that process is opened
 * For informational purposes only
 */

void GnuplotProcess::started()
{
    qInfo() << Q_FUNC_INFO;
}



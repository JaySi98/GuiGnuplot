/*****************************************************
 * FILE NAME                                         *
 * gnuplotprocess.h                                  *
 *                                                   *
 * PURPOSE                                           *
 * GnuplotProcess class handles communication with   *
 * Gnuplot program. Sends comands from QString script*
 * and informs about possible errors.                *
 *****************************************************/

#ifndef GNUPLOTPROCESS_H
#define GNUPLOTPROCESS_H

#include <QObject>
#include <QProcess>
#include <QtDebug>
#include <QSysInfo>
#include <QFile>
#include <QDataStream>
#include <QList>
#include "enums.h"

class GnuplotProcess: public QObject
{
    Q_OBJECT

public:
    explicit GnuplotProcess(QObject *parent = nullptr);
    ~GnuplotProcess();

    QString getOperatingSystem();
    void    setScript(QString script);
    QString getScript();
    void    generateGraph();
    void    help(QString command);

private:
    QString  operating_system;
    QProcess process;
    QString  script;

    QString    getProcess();
    void       addCommandsToScript();
    OutputType checkStandardOutput(QByteArray input);
    void       writeCommand(QString input);


signals:
    void output(QString data, OutputType type);


public slots:
    void start();
    void stop();


private slots:
    void finished(int exitcode, QProcess::ExitStatus exit_status);
    void started();
    void stateChanged(QProcess::ProcessState new_state);
    void errorOccurred(QProcess::ProcessError error);
    void readyReadStandardOutput();    
};

#endif // GNUPLOTPROCESS_H

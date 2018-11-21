<<<<<<< HEAD
#include <QtCore>
#include "Packet.h"
=======
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTime>
#include <QtDebug>
#include <QFile>
>>>>>>> cb9c2ec5acb5673a798ebe51b6104512a78eed31

void help();
void interval(int millisecondsToWait);
void start(pcpp::Packet packet, int num, int ms);
void delay(int millisecondsToWait );
pcpp::Packet parsePacket(QFile* file);
int send(pcpp::Packet packet);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption helpOption(QStringList() << "h" << "help",
                QCoreApplication::translate("main", "Usage Help"));
    parser.addOption(helpOption);
    QCommandLineOption targetInputOption(QStringList() << "f" << "file",
                QCoreApplication::translate("main", "Specify a file"));
    parser.addOption(targetInputOption);
    QCommandLineOption intervalOption(QStringList() << "i" << "interval" ,
                QCoreApplication::translate("main", "chosse a interval in ms"));
    parser.addOption(intervalOption);
    QCommandLineOption totalOption(QStringList() << "t" << "total",
                QCoreApplication::translate("main", "amount of time to send package"));
    parser.addOption(totalOption);

    parser.process(a);

    QStringList args = parser.optionNames();

    int ms=1000;
    int total=1;
    pcpp::Packet packet;

    if(args.contains("h") || args.contains("help")){
        help();
        return 0;
    }

    if(args.contains("f") || args.contains("file")){
        QFile file(parser.value(targetInputOption));
        parsePacket(&file);
    }
    else {
        help();
        return 0;
    }

    if(args.contains("i") || args.contains("interval")){
       ms =  parser.value(intervalOption).toInt();
    }

    if(args.contains("t") || args.contains("total")){
        total =  parser.value(totalOption).toInt();
    }

    start(packet, total, ms);

    return 0;
}

void help()
{
    qDebug() << "Usage:\n-f\t--file\t\tChoose a file containing the packet to send" <<
                "\n-i\t--interval\tInterval between packets sent in ms (Default 1000)" <<
                "\n-t\t--total\t\tTotal packets to send (Default 1)" ;
}

<<<<<<< HEAD
=======
void chooseFile(QString file){
    qDebug() << "file";

    if (file.isEmpty()){
        qDebug() << "Choose a file";
    }else{
        QFile ficheiro(file);

        if (!ficheiro.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Not possible to open file";
            return;
        }else{
            qDebug() << "File open";
            QString content = ficheiro.readAll();
            ficheiro.close();
        }
    }
}
>>>>>>> cb9c2ec5acb5673a798ebe51b6104512a78eed31

void interval(int millisecondsToWait)
{
    delay(millisecondsToWait);
}

void delay(int millisecondsToWait )
{
    QTime *dieTime = new QTime(QTime::currentTime().addMSecs( millisecondsToWait ));
    while( QTime::currentTime() < *dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

pcpp::Packet parsePacket(QFile* file)
{
    pcpp::Packet packet;
    return packet;
}

void start(pcpp::Packet packet, int num, int ms)
{
    qDebug() << "Sending packet(s)...";
    for(int i=1;i<=num;i++)
    {
        if(send(packet)){
            qDebug() << "Packet sent.";
        }
        if(i!= num)
        {
            interval(ms);
        }
    }
    qDebug() << "Sent" << num << "packet(s).";
}

int send(pcpp::Packet packet)
{
    return 1;
}

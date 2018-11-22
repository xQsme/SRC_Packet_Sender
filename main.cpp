#include <QtCore>
#include <QNetworkInterface>
#include "Packet.h"
#include "PcapLiveDeviceList.h"
#include "PcapFileDevice.h"
#include "IPv4Layer.h"

void help();
void interval(int millisecondsToWait);
void start(QList<pcpp::Packet> packets, int ms, pcpp::PcapLiveDevice* dev);
void delay(int millisecondsToWait);
int parsePackets(QList<pcpp::Packet>* packets, QString file);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption helpOption(QStringList() << "h" << "help",
                QCoreApplication::translate("main", "Usage Help"));
    parser.addOption(helpOption);
    QCommandLineOption targetInputOption(QStringList() << "f" << "file",
                QCoreApplication::translate("main", "Specify a file"),
                QCoreApplication::translate("main", "file"));
    parser.addOption(targetInputOption);
    QCommandLineOption intervalOption(QStringList() << "i" << "interval" ,
                QCoreApplication::translate("main", "chosse a interval in ms"));
    parser.addOption(intervalOption);

    parser.process(a);

    QStringList args = parser.optionNames();

    int ms=100;
    QList<pcpp::Packet> packets;

    if(args.contains("h") || args.contains("help")){
        help();
        return 0;
    }

    if(args.contains("f") || args.contains("file")){
        QString file = parser.value(targetInputOption);
        if(parsePackets(&packets, file)==0)
        {
            return -1;
        }
    }
    else {
        help();
        return 0;
    }

    if(args.contains("i") || args.contains("interval")){
       ms =  parser.value(intervalOption).toInt();
    }
    std::string interfaceIPAddr;
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
                 interfaceIPAddr =  address.toString().toStdString();
    }
    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr.c_str());
    start(packets, ms, dev);

    return 0;
}

void help()
{
    qDebug() << "Usage:\n-f\t--file\t\tChoose a file containing the packet to send" <<
                "\n-i\t--interval\tInterval between packets sent in ms (Default 100)";
}

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

int parsePackets(QList<pcpp::Packet>* packets, QString file)
{
    pcpp::IFileReaderDevice* reader = pcpp::IFileReaderDevice::getReader(file.toLocal8Bit().data());
    if (reader == NULL)
    {
        qDebug() << "Invalid packet file.";
        return 0;
    }
    if (!reader->open())
    {
        qDebug() << "Cannot open packet reader.";
        return 0;
    }
    pcpp::RawPacket rawPacket;
    while (reader->getNextPacket(rawPacket))
    {
        packets->append(pcpp::Packet(&rawPacket));
    }
    reader->close();
    return 1;
}

void start(QList<pcpp::Packet> packets, int ms, pcpp::PcapLiveDevice* dev)
{
    qDebug() << "Sending packet(s)...";
    QString IP = QString::fromStdString(packets[0].getLayerOfType<pcpp::IPv4Layer>()->getSrcIpAddress().toString());
    int count=0;
    dev->open();
    foreach(pcpp::Packet packet, packets)
    {
        if(dev->sendPacket(&packet))
        {
            qDebug() << "Packet sent to" << IP;
            count++;
        }
        else
        {
            qDebug() << "Failed sending packet to" << IP;
        }
        interval(ms);
    }
    dev->close();
    qDebug() << "Sent" << count << "packet(s).";
}

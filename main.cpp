#include <QtCore>
#include <QNetworkInterface>
#include "Packet.h"
#include "PcapLiveDeviceList.h"
#include "PcapFileDevice.h"
#include "IPv4Layer.h"

void help();
int start(QList<pcpp::Packet> packets, int ms, pcpp::PcapLiveDevice* dev);
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
                QCoreApplication::translate("main", "chosse a interval in ms"),
                QCoreApplication::translate("main", "interval"));
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

    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
    QList<QString> lst;
    int i= 1;

    foreach(QNetworkInterface inter,allInterfaces){
        QString msg = QString::number(i)+" -> "+inter.humanReadableName();
        QList<QNetworkAddressEntry> address= inter.addressEntries();

        foreach (const QNetworkAddressEntry &add, address){
            if (add.ip().protocol() == QAbstractSocket::IPv4Protocol){
                msg.append(" - "+add.ip().toString());
                lst.append(msg);
            }
        }
        i++;
    }
    QString final = "Enter option ("+QString::number(1)+"-"+QString::number(allInterfaces.length())+"):";
    int opcao;
    do{
        qDebug() << "Select an interface:";
        foreach(QString std , lst){
            qDebug() << std;
        }
        qDebug() << final;
        scanf("%d",&opcao);
    }while(opcao<1 || opcao >allInterfaces.length());

    QString selected = lst.value(opcao-1);
    QString ip = selected.split("- ")[1];
    interfaceIPAddr = ip.toStdString();

    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr.c_str());
    if(dev == NULL)
    {
        qDebug() << "Unable to create device.";
    }
    return start(packets, ms, dev);
}

void help()
{
    qDebug() << "Usage:\n-f\t--file\t\tChoose a file containing the packet to send" <<
                "\n-i\t--interval\tInterval between packets sent in ms (Default 100)";
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

int start(QList<pcpp::Packet> packets, int ms, pcpp::PcapLiveDevice* dev)
{
    qDebug() << "Sending packet(s)...";
    QString IP;
    int count=0;
    if(!dev->open()){
        qDebug() << "Unable to open device, please run with elevated privileges.";
        return -1;
    }
    pcpp::IPv4Layer* ipLayer;

    foreach(pcpp::Packet packet, packets)
    {
        ipLayer = packet.getLayerOfType<pcpp::IPv4Layer>();
        ipLayer->setSrcIpAddress(pcpp::IPv4Address(IP.toStdString()));
        packet.computeCalculateFields();
        IP = QString::fromStdString(ipLayer->getDstIpAddress().toString());
        if(dev->sendPacket(&packet))
        {
            qDebug() << "Packet sent to" << IP;
            count++;
        }
        else
        {
            qDebug() << "Failed sending packet to" << IP;
        }
        QThread::msleep(ms);
    }
    dev->close();
    qDebug() << "Sent" << count << "packet(s).";

    return 0;
}

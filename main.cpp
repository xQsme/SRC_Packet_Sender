#include <QtCore>
#include <QNetworkInterface>
#include "Packet.h"
#include "PcapLiveDeviceList.h"
#include "PcapFileDevice.h"
#include "IPv4Layer.h"
#include "thread.h"

void help();
int parsePackets(QList<pcpp::Packet>* packets, QString file);
void start(QList<pcpp::Packet> packets, int ms, pcpp::PcapLiveDevice* dev, int repeat, QString src, int threads);
void thread(QList<pcpp::Packet> packets, int ms, pcpp::PcapLiveDevice* dev, int repeat, pcpp::IPv4Layer* ipLayer);


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
                QCoreApplication::translate("main", "chosse an interval in ms"),
                QCoreApplication::translate("main", "interval"));
    parser.addOption(intervalOption);
    QCommandLineOption repeatOption(QStringList() << "r" << "repeat" ,
                QCoreApplication::translate("main", "specify how many times to repeat"),
                QCoreApplication::translate("main", "repeat"));
    parser.addOption(repeatOption);
    QCommandLineOption threadsOption(QStringList() << "t" << "threads" ,
                QCoreApplication::translate("main", "specify how many threads to use"),
                QCoreApplication::translate("main", "threads"));
    parser.addOption(threadsOption);


    parser.process(a);

    QStringList args = parser.optionNames();

    int ms=100;
    int repeat = 1;
    int threads = 1;
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
    if(args.contains("r") || args.contains("repeat")){
       repeat =  parser.value(repeatOption).toInt();
    }
    if(args.contains("t") || args.contains("threads")){
       threads =  parser.value(threadsOption).toInt();
    }
    std::string interfaceIPAddr;
    QList<QString> list;
    int i= 1;
    foreach(QNetworkInterface inter,QNetworkInterface::allInterfaces()){
        foreach (const QNetworkAddressEntry &add, inter.addressEntries()){
            if (add.ip().protocol() == QAbstractSocket::IPv4Protocol){
                QString msg = QString::number(i)+" -> "+inter.humanReadableName();
                msg.append(" - "+add.ip().toString());
                list.append(msg);
                i++;
            }
        }
    }
    QString final = "Enter option ("+QString::number(1)+"-"+QString::number(list.length())+"):";
    int opcao;
    do{
        qDebug() << "Select an interface:";
        foreach(QString std , list){
            qDebug() << std;
        }
        qDebug() << final;
        scanf("%d",&opcao);
    }while(opcao<1 || opcao > list.length());

    QString ip = list.value(opcao-1).split("- ")[1];
    interfaceIPAddr = ip.toStdString();
    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr.c_str());
    if(dev == NULL)
    {
        qDebug() << "Unable to create device.";
        return -1;
    }
    start(packets, ms, dev, repeat, ip, threads);

    return 0;
}

void help()
{
    qDebug() << "Usage:\n-f\t--file\t\tChoose a file containing the packet(s) to send" <<
                "\n-i\t--interval\tInterval between packets sent in ms (Default 100)" <<
                "\n-r\t--repeat\tSpecify how many times to repeat (Default 1)" <<
                "\n-t\t--threads\tSpecify how many threads to use (Default 1)";
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

void start(QList<pcpp::Packet> packets, int ms, pcpp::PcapLiveDevice* dev, int repeat, QString src, int threads)
{
    if(!dev->open()){
        qDebug() << "Unable to open device, please run with elevated privileges.";
        return;
    }
    int count=0;
    pcpp::IPv4Layer* ipLayer;
    for(int i=0; i<packets.length(); i++)
    {
        ipLayer = packets[i].getLayerOfType<pcpp::IPv4Layer>();
        ipLayer->setSrcIpAddress(pcpp::IPv4Address(src.toStdString()));
        packets[i].computeCalculateFields();
        count++;
    }
    int remainder = repeat%threads;
    int each = repeat/threads;
    QList<Thread*> t;
    for(int i=0; i<threads; i++)
    {
        int current=each;
        if(remainder > 0)
        {
            current++;
            remainder--;
        }
        t.append(new Thread(&packets, ms, dev, current));
    }
    qDebug() << "Sending packet(s) with" << threads << "threads...";
    QElapsedTimer elapsed;
    elapsed.start();
    for(int i=0; i<threads; i++)
    {
        t[i]->start();
    }
    for(int i=0; i<threads; i++)
    {
        t[i]->wait();
    }
    dev->close();
    qDebug() << "Sent" << count*repeat << "packet(s) in" << elapsed.elapsed() << "ms.";
}

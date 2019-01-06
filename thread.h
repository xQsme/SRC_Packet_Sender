#ifndef THREAD_H
#define THREAD_H

#include <QtCore>
#include "Packet.h"
#include "PcapLiveDeviceList.h"

class Thread : public QThread
{
public:
    Thread(QList<pcpp::Packet>* packets, int ms, pcpp::PcapLiveDevice* dev, int repeat);
    void run();
private:
    QList<pcpp::Packet>* packets;
    int ms;
    pcpp::PcapLiveDevice* dev;
    int repeat;
};

#endif // THREAD_H

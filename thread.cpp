#include "thread.h"

Thread::Thread(QList<pcpp::Packet>* packets, int ms, pcpp::PcapLiveDevice* dev, int repeat)
{
    this->packets=packets;
    this->ms=ms;
    this->dev=dev;
    this->repeat=repeat;
}

void Thread::run()
{
    for (int i=0; i<repeat; ++i)
    {
        foreach(pcpp::Packet packet, *packets)
        {
            dev->sendPacket(&packet);
            QThread::msleep(ms);
        }
    }
}

# SRC_Packet_Sender

Requires qt5 and libpcap

The point of this program is to send previously saved packets

```
https://github.com/xQsme/SRC_Packet_Sender
cd SRC_Packet_Sender/release
./packetSender
```

Usage:
```
-f   --file            Choose a file containing the packet(s) to send
-i   --interval       Interval between packets sent in ms (Default 100)
-r   --repeat         Specify how many times to repeat (Default 1)
-t   --threads        Specify how many threads to use (Default 1)
```

Common Usage:
```
./packetSender -f pathToFile
```

#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <buffer.h>

BufData mk_pkt(BufData data, const char id)
{
    uint16_t len = data.len + sizeof(char);

    char *buf = new char[len];
    buf[0] = id;
    for (int i = 0; i < data.len; i++)
    {
        buf[i + 1] = data.data[i];
    }

    return BufData(len, buf);
}

#endif
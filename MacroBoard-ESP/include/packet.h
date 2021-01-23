#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <buffer.h>
#include <random>

static const uint32_t MAX_UINT = 1 << 32;

uint32_t mk_pkt(BufDataWriter &writer, const char id)
{
    int32_t _uid = rand();
    uint32_t uid = 0;
    if (_uid < 0)
    {
        uid = _uid + MAX_UINT;
    }
    else
    {
        uid = _uid;
    }

    writer.WriteChar(id);
    writer.WriteU32(uid);

    return uid;
}

#endif
#ifndef PAYLOADS_H
#define PAYLOADS_H

#include <packet.h>
#include <serialize.h>

class ButtonPayload : public ISerializer
{
public:
    char id;
    char pattern;

public:
    ButtonPayload(BufData &data)
    {
        from_bytes(data);
    }

    ButtonPayload(char id_, char pattern_)
    {
        id = id_;
        pattern = pattern_;
    }

    BufData to_bytes()
    {
        char *buf = new char[2];
        buf[0] = id;
        buf[1] = pattern;

        return BufData(2, buf);
    }

    void from_bytes(BufData &data)
    {
        id = data.data[0];
        pattern = data.data[1];
    }
};

class StringPayload : public ISerializer
{
public:
    String msg;

public:
    StringPayload(String msg_)
    {
        msg = msg_;
    }

    StringPayload(BufData &data)
    {
        from_bytes(data);
    }

    void from_bytes(BufData &data)
    {
        msg = String(data.data);
    }

    BufData to_bytes()
    {
        char *buf = (char *)msg.c_str();
        return BufData(msg.length(), buf);
    }
};

class BitmapPayload : public ISerializer
{
public:
    int16_t width;
    int16_t height;
    uint8_t *data;
    uint16_t data_len;

public:
    void from_bytes(BufData &buf)
    {
        width = buf.data[0];
        height = buf.data[1];
        data_len = buf.len - sizeof(int16_t) * 2;
        data = (uint8_t *)(buf.data + sizeof(int16_t) * 2);
    }

    BufData to_bytes()
    {
        char *buf = new char[data_len + sizeof(int16_t) * 2];
        buf[0] = width & 0x00ff;
        buf[1] = width & 0xff00 >> 8;
        buf[2] = height & 0x00ff;
        buf[3] = height & 0xff00 >> 8;

        char *databuf = buf + sizeof(int16_t) * 2;
        memcpy(buf, &databuf, data_len);

        return BufData(data_len + sizeof(int16_t) * 2, databuf);
    }
};

#endif
#ifndef BUFFER_H
#define BUFFER_H

#include <MsgPack.h>

struct BufData
{
    uint16_t len;
    const char *data;

    BufData(uint16_t len_, char *data_)
    {
        len = len_;
        data = data_;
    }
};

struct BufDataReader
{
    MsgPack::Unpacker unpacker;

    BufDataReader(const BufData &data)
    {
        this->unpacker = MsgPack::Unpacker();
        this->unpacker.feed(reinterpret_cast<const uint8_t *>(data.data), data.len);
    }

    u32_t ReadU32()
    {
        return this->unpacker.unpackUInt32();
    }

    char ReadChar()
    {
        return (char)this->unpacker.unpackUInt8();
    }

    String ReadString()
    {
        return this->unpacker.unpackString();
    }
};

struct BufDataWriter
{
    MsgPack::Packer packer;

    BufDataWriter()
    {
        this->packer = MsgPack::Packer();
    }

    void WriteU32(uint32_t value)
    {
        this->packer.packUInt32(value);
    }

    void WriteChar(char value)
    {
        this->packer.packUInt8((uint8_t)value);
    }

    void WriteString(String value)
    {
        this->packer.packString(value);
    }

    void WriteBufData(BufData &bufData)
    {
        this->packer.pack(bufData.data, bufData.len);
    }

    BufData GetData()
    {
        auto &packet = this->packer.packet();

        char *data = (char *)packet.data();
        size_t len = packet.size();

        return BufData(len, data);
    }
};

#endif
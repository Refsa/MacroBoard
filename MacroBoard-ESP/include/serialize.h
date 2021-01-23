#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <buffer.h>

class ISerializer
{
public:
    virtual void to_bytes(BufDataWriter &writer);
    virtual void from_bytes(BufDataReader &reader);
};

#endif
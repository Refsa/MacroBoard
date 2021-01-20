#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <buffer.h>

class ISerializer
{
public:
    virtual BufData to_bytes();
    virtual void from_bytes(BufData &data);
};

#endif
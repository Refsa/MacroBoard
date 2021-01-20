#ifndef BUFFER_H
#define BUFFER_H

struct BufData
{
    uint16_t len;
    char *data;
    BufData(uint16_t len_, char *data_)
    {
        len = len_;
        data = data_;
    }
};

#endif
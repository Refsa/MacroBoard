#ifndef RANGE_H
#define RANGE_H

template <class T>
struct Range
{
    T min;
    T max;

    Range(T min, T max)
    {
        this->min = min;
        this->max = max;
    }
};

#endif
#ifndef SLIDE_POT_H
#define SLIDE_POT_H

#include <Arduino.h>

class SlidePot
{
public:
    SlidePot(uint8_t pin)
    {
        this->pin = pin;
        this->reading = 0;
    }

    bool Read(float &reading)
    {
        uint32_t current_time = millis();

        uint16_t last_read = this->reading;
        this->reading = analogRead(this->pin) * 8;
        reading = static_cast<float>(this->reading) / MAX_READING;

        bool force_read = ((current_time - this->last_reading_time) > READ_TIMEOUT);
        last_reading_time = current_time;

        return (abs(last_read - this->reading) > SENSOR_LEEWAY) || force_read;
    }

    uint16_t LastReading()
    {
        return this->reading;
    }

private:
    const uint16_t SENSOR_LEEWAY = 350;
    const float MAX_READING = 4096 * 8;
    const uint32_t READ_TIMEOUT = 80;

    uint8_t pin;
    uint16_t reading;
    uint32_t last_reading_time;
};

#endif
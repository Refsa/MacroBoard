#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>
#include <CircularBuffer.h>

// 12 - 13

class RotaryEncoder
{
public:
    typedef enum State
    {
        NONE,
        LEFT,
        RIGHT
    };

    typedef void (*callback_t)(State state);

public:
    RotaryEncoder(uint8_t pin_l, uint8_t pin_r)
    {
        this->pin_l = pin_l;
        this->pin_r = pin_r;

        for (int i = 0; i < SEQ_LEN; i++)
            this->sequence.push(0b0011);
    }

    void OnStateChanged(callback_t callback)
    {
        this->state_changed_callback = callback;
    }

    void Loop()
    {
        uint64_t read_time = millis();
        if (read_time - this->last_read_time < 2)
        {
            return;
        }
        this->last_read_time = read_time;

        int pl = digitalRead(this->pin_l);
        int pr = digitalRead(this->pin_r);

        char enc = 0b0000;
        enc |= (1 << 0) * pl;
        enc |= (1 << 1) * pr;

        if (this->sequence.isFull())
        {
            this->sequence.pop();
        }
        this->sequence.unshift(enc);

        char sum = 0;
        for (int i = 0; i < SEQ_LEN; i++)
        {
            if (this->sequence[i] == 0b0011)
            {
                sum += 1;
            }
        }

        State prev_state = this->last_state;
        if (sum >= SEQ_LEN)
        {
            this->last_state = State::NONE;
        }
        else
        {
            for (int i = 0; i < SEQ_LEN; i++)
            {
                if (this->sequence[i] == 0b0001)
                {
                    if (this->sequence[i + 1] == 0b0000)
                    {
                        this->last_state = State::LEFT;
                        break;
                    }
                    else if (this->sequence[i + 1] == 0b0011)
                    {
                        this->last_state = State::RIGHT;
                        break;
                    }
                }
            }
        }

        if (this->last_state != prev_state)
        {
            state_changed_callback(this->last_state);
        }
    }

    State GetState()
    {
        return this->last_state;
    }

public:
    static String StateToName(const State &state)
    {
        switch (state)
        {
        case State::NONE:
            return "None";
        case State::LEFT:
            return "Left";
        case State::RIGHT:
            return "Right";
        }

        return "NIL";
    }

    static int8_t StateToID(const State &state)
    {
        switch (state)
        {
        case State::LEFT:
            return -1;
        case State::RIGHT:
            return 1;
        }

        return 0;
    }

private:
    const static size_t SEQ_LEN = 4;

    uint8_t pin_l;
    uint8_t pin_r;

    State last_state;
    uint64_t last_read_time;

    callback_t state_changed_callback;

    CircularBuffer<char, SEQ_LEN> sequence;
};

#endif
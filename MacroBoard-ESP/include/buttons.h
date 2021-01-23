#ifndef BUTTONS_H
#define BUTTONS_H

#include <button_array.h>
#include <range.h>

// Voltage Readings
// 3050 2225 1428 640
// 3027 2200 1400 600
// 3027 2200 1400 600

const unsigned int trBtnPin = 33;
const unsigned int mrBtnPin = 32;
const unsigned int brBtnPin = 35;

const int bArrayTopID = 0;
const int bArrayMiddleID = 1;
const int bArrayBottomID = 2;

typedef void (*button_pressed_callback_t)(uint8_t btn, BfButton::press_pattern_t pattern);
button_pressed_callback_t button_pressed_callback;

void ButtonPressedCallback(uint8_t btn_id, BfButton::press_pattern_t pattern)
{
    button_pressed_callback(btn_id, pattern);
}

void ButtonPressedCallbackTR(BfButton *btn, BfButton::press_pattern_t pattern)
{
    uint8_t btn_id = btn->getID();
    ButtonPressedCallback(btn_id + bArrayTopID * 4, pattern);
}

void ButtonPressedCallbackMR(BfButton *btn, BfButton::press_pattern_t pattern)
{
    uint8_t btn_id = btn->getID();
    ButtonPressedCallback(btn_id + bArrayMiddleID * 4, pattern);
}

void ButtonPressedCallbackBR(BfButton *btn, BfButton::press_pattern_t pattern)
{
    uint8_t btn_id = btn->getID();
    ButtonPressedCallback(btn_id + bArrayBottomID * 4, pattern);
}

ButtonArray bArrayTop(
    trBtnPin, ButtonPressedCallbackTR,
    Range<uint16_t>(2950, 3150),
    Range<uint16_t>(2100, 2300),
    Range<uint16_t>(1300, 1500),
    Range<uint16_t>(550, 700));

ButtonArray bArrayMiddle(
    mrBtnPin, ButtonPressedCallbackMR,
    Range<uint16_t>(2950, 3150),
    Range<uint16_t>(2100, 2300),
    Range<uint16_t>(1300, 1500),
    Range<uint16_t>(550, 700));

ButtonArray bArrayBottom(
    brBtnPin, ButtonPressedCallbackBR,
    Range<uint16_t>(2950, 3150),
    Range<uint16_t>(2100, 2300),
    Range<uint16_t>(1300, 1500),
    Range<uint16_t>(550, 700));

void StartButtons()
{
    bArrayTop.Begin();
    bArrayMiddle.Begin();
    bArrayBottom.Begin();
}

void LoopButtons()
{
    bArrayTop.Loop();
    bArrayMiddle.Loop();
    bArrayBottom.Loop();
}

uint8_t button_state_id(const BfButton::press_pattern_t &pattern)
{
    switch (pattern)
    {
    case BfButton::SINGLE_PRESS:
        return 0;
    case BfButton::DOUBLE_PRESS:
        return 1;
    case BfButton::LONG_PRESS:
        return 2;
    }
}
#endif
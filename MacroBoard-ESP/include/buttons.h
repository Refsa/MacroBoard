#ifndef BUTTONS_H
#define BUTTONS_H

#include <button_array.h>
#include <range.h>

// Voltage Readings
// 3050 2225 1428 640
// 3027 2200 1400 600
// 3027 2200 1400 600

const uint8_t trBtnPin = 33;
const uint8_t mrBtnPin = 32;
const uint8_t brBtnPin = 35;

const uint8_t bArrayTopID = 0;
const uint8_t bArrayMiddleID = 1;
const uint8_t bArrayBottomID = 2;

typedef void (*button_pressed_callback_t)(uint8_t btn, BfButton::press_pattern_t pattern);
static button_pressed_callback_t button_pressed_callback;

class ButtonCallbacks
{
public:
    static void ButtonPressedCallback(uint8_t btn_id, BfButton::press_pattern_t pattern)
    {
        button_pressed_callback(btn_id, pattern);
    }

    static void ButtonPressedCallbackTR(BfButton *btn, BfButton::press_pattern_t pattern)
    {
        uint8_t btn_id = btn->getID();
        ButtonCallbacks::ButtonPressedCallback(btn_id + bArrayTopID * 4, pattern);
    }

    static void ButtonPressedCallbackMR(BfButton *btn, BfButton::press_pattern_t pattern)
    {
        uint8_t btn_id = btn->getID();
        ButtonCallbacks::ButtonPressedCallback(btn_id + bArrayMiddleID * 4, pattern);
    }

    static void ButtonPressedCallbackBR(BfButton *btn, BfButton::press_pattern_t pattern)
    {
        uint8_t btn_id = btn->getID();
        ButtonCallbacks::ButtonPressedCallback(btn_id + bArrayBottomID * 4, pattern);
    }
};

struct ButtonMatrix
{
    ButtonArray *barray;

    void Setup()
    {
        barray = new ButtonArray[3]{
            ButtonArray(
                trBtnPin, ButtonCallbacks::ButtonPressedCallbackTR,
                Range<uint16_t>(2950, 3150),
                Range<uint16_t>(2100, 2300),
                Range<uint16_t>(1300, 1500),
                Range<uint16_t>(550, 700)),
            ButtonArray(
                mrBtnPin, ButtonCallbacks::ButtonPressedCallbackMR,
                Range<uint16_t>(2950, 3150),
                Range<uint16_t>(2100, 2300),
                Range<uint16_t>(1300, 1500),
                Range<uint16_t>(550, 700)),
            ButtonArray(
                brBtnPin, ButtonCallbacks::ButtonPressedCallbackBR,
                Range<uint16_t>(2950, 3150),
                Range<uint16_t>(2100, 2300),
                Range<uint16_t>(1300, 1500),
                Range<uint16_t>(550, 700))};
    }

    void LoopButtons()
    {
        for (int i = 0; i < 2; i++)
        {
            barray[i].Loop();
        }
    }
};

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
#ifndef BUTTON_ARRAY_H
#define BUTTON_ARRAY_H

#include <BfButtonManager.h>
#include <BfButton.h>
#include <range.h>

class ButtonArray
{
public:
    ButtonArray() {}

    ButtonArray(uint8_t pin, BfButton::callback_t callback, const Range<uint16_t> &btn1, const Range<uint16_t> &btn2, const Range<uint16_t> &btn3, const Range<uint16_t> &btn4)
    {
        this->manager = BfButtonManager(pin, 4);
        this->id = pin;

        this->btn1 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 0);
        this->btn2 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 1);
        this->btn3 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 2);
        this->btn4 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 3);

        this->btn1.onPress(callback).onDoublePress(callback).onPressFor(callback, 500);
        this->btn2.onPress(callback).onDoublePress(callback).onPressFor(callback, 500);
        this->btn3.onPress(callback).onDoublePress(callback).onPressFor(callback, 500);
        this->btn4.onPress(callback).onDoublePress(callback).onPressFor(callback, 500);

        this->manager.addButton(&this->btn1, btn1.min, btn1.max);
        this->manager.addButton(&this->btn2, btn2.min, btn2.max);
        this->manager.addButton(&this->btn3, btn3.min, btn3.max);
        this->manager.addButton(&this->btn4, btn4.min, btn4.max);

        this->manager.begin();
    }

    void Loop()
    {
        this->manager.loop();
    }

private:
    uint8_t id;
    BfButtonManager manager = BfButtonManager(0, 4);
    BfButton btn1 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 0);
    BfButton btn2 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 0);
    BfButton btn3 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 0);
    BfButton btn4 = BfButton(BfButton::ANALOG_BUTTON_ARRAY, 0);
};

#endif
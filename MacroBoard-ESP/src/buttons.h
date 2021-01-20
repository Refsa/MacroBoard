#ifndef BUTTONS_H
#define BUTTONS_H

#include <BfButtonManager.h>
#include <BfButton.h>

// Voltage Readings
// 3050 2225 1428 640
// 3027 2200 1400 600
// 3027 2200 1400 600

const unsigned int trBtnPin = 33;
const unsigned int mrBtnPin = 32;
const unsigned int brBtnPin = 35;

BfButtonManager button_manager_tr(trBtnPin, 4);
BfButtonManager button_manager_mr(mrBtnPin, 4);
BfButtonManager button_manager_br(brBtnPin, 4);

BfButton btn_tr1(BfButton::ANALOG_BUTTON_ARRAY, 0);
BfButton btn_tr2(BfButton::ANALOG_BUTTON_ARRAY, 1);
BfButton btn_tr3(BfButton::ANALOG_BUTTON_ARRAY, 2);
BfButton btn_tr4(BfButton::ANALOG_BUTTON_ARRAY, 3);

BfButton btn_mr1(BfButton::ANALOG_BUTTON_ARRAY, 0);
BfButton btn_mr2(BfButton::ANALOG_BUTTON_ARRAY, 1);
BfButton btn_mr3(BfButton::ANALOG_BUTTON_ARRAY, 2);
BfButton btn_mr4(BfButton::ANALOG_BUTTON_ARRAY, 3);

BfButton btn_br1(BfButton::ANALOG_BUTTON_ARRAY, 0);
BfButton btn_br2(BfButton::ANALOG_BUTTON_ARRAY, 1);
BfButton btn_br3(BfButton::ANALOG_BUTTON_ARRAY, 2);
BfButton btn_br4(BfButton::ANALOG_BUTTON_ARRAY, 3);

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

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern)
{
    Serial.print(btn->getID());
    switch (pattern)
    {
    case BfButton::SINGLE_PRESS:
        Serial.println(" pressed.");
        break;
    case BfButton::DOUBLE_PRESS:
        Serial.println(" double pressed.");
        break;
    case BfButton::LONG_PRESS:
        Serial.println(" long pressed.");
        break;
    }
}

struct ButtonConfig
{
    BfButton &btn;
    uint16_t min;
    uint16_t max;
};

void setup_button_row(BfButtonManager &mgr, BfButton::callback_t callback, ButtonConfig *btns)
{
    btns[0].btn.onPress(callback).onDoublePress(callback).onPressFor(callback, 1000);

    btns[1].btn.onPress(callback).onDoublePress(callback).onPressFor(callback, 1000);

    btns[2].btn.onPress(callback).onDoublePress(callback).onPressFor(callback, 1000);

    btns[3].btn.onPress(callback).onDoublePress(callback).onPressFor(callback, 1000);

    mgr.addButton(&btns[0].btn, btns[0].min, btns[0].max);
    mgr.addButton(&btns[1].btn, btns[1].min, btns[1].max);
    mgr.addButton(&btns[2].btn, btns[2].min, btns[2].max);
    mgr.addButton(&btns[3].btn, btns[3].min, btns[3].max);
}

void setup_buttons(BfButton::callback_t callback_tr, BfButton::callback_t callback_mr, BfButton::callback_t callback_br)
{
    ButtonConfig *top_row = new ButtonConfig[4]{
        ButtonConfig{
            .btn = btn_tr1, .min = 2950, .max = 3150},
        ButtonConfig{
            .btn = btn_tr2, .min = 2100, .max = 2300},
        ButtonConfig{
            .btn = btn_tr3, .min = 1300, .max = 1500},
        ButtonConfig{
            .btn = btn_tr4, .min = 550, .max = 700},
    };

    ButtonConfig *middle_row = new ButtonConfig[4]{
        ButtonConfig{
            .btn = btn_mr1, .min = 2950, .max = 3150},
        ButtonConfig{
            .btn = btn_mr2, .min = 2100, .max = 2300},
        ButtonConfig{
            .btn = btn_mr3, .min = 1300, .max = 1500},
        ButtonConfig{
            .btn = btn_mr4, .min = 550, .max = 700},
    };

    ButtonConfig *bottom_row = new ButtonConfig[4]{
        ButtonConfig{
            .btn = btn_br1, .min = 2950, .max = 3150},
        ButtonConfig{
            .btn = btn_br2, .min = 2100, .max = 2300},
        ButtonConfig{
            .btn = btn_br3, .min = 1300, .max = 1500},
        ButtonConfig{
            .btn = btn_br4, .min = 550, .max = 700},
    };

    setup_button_row(button_manager_tr, callback_tr, top_row);
    setup_button_row(button_manager_mr, callback_mr, middle_row);
    setup_button_row(button_manager_br, callback_br, bottom_row);

    button_manager_tr.begin();
    button_manager_mr.begin();
    button_manager_br.begin();
}

#endif
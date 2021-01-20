/* #include <Arduino.h>
#include <BfButton.h>

const char *ssid = "M10";
const char *password = "M10brunost310";

const unsigned int btnPin = 27;
BfButton btn(BfButton::ANALOG_BUTTON_ARRAY, btnPin, false, HIGH);

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

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    Serial.println("Program Start");

    btn.onPress(pressHandler)
        .onDoublePress(pressHandler)
        .onPressFor(pressHandler, 1000);

    // connectToNetwork(ssid, password);
    // wifiServer.begin();
}

void loop()
{
    btn.read();

    // WiFiClient client = wifiServer.available();

    // if (client)
    // {
    //     server(&client);
    // }
} */
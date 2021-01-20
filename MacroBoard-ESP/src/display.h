#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, 23, 5, 2, 4, 15);

void setup_display()
{
    Serial.begin(115200);
    Serial.println("Program Start");

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    Serial.println("Display Ready");
    delay(2000);
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void display_clear()
{
    display.clearDisplay();
}

void display_print(const char* data, uint8_t cursor_x, uint8_t cursor_y)
{
    display.setCursor(cursor_x, cursor_y);
    display.println(data);
    display.display();
}

void display_print(const String &data, uint8_t cursor_x, uint8_t cursor_y)
{
    display.setCursor(cursor_x, cursor_y);
    display.println(data);
    display.display();
}
#endif
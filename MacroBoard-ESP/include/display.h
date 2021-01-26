#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <logo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, 23, 5, 2, 4, 15);

struct DisplayBuffer
{
    String lines[6];
};

void setup_display()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    Serial.println("Display Ready");
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.dim(true);
}

void display_clear()
{
    display.clearDisplay();
}

void display_print(const char *data, uint8_t cursor_x, uint8_t cursor_y)
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

void set_dbuffer_line(DisplayBuffer &dbuffer, const String &data, uint8_t line)
{
    dbuffer.lines[line] = data;
}

void clear_dbuffer(DisplayBuffer &dbuffer, uint8_t from, uint8_t to)
{
    for (int i = from; i <= to; i++)
    {
        dbuffer.lines[i] = "";
    }
}

void display_dbuffer(DisplayBuffer &buffer)
{
    display.fillRect(0, 0, 128, 64, BLACK);
    for (int i = 0; i < 6; i++)
    {
        display.setCursor(0, i * 10 + 18);
        display.println(buffer.lines[i]);
    }
    display.drawXBitmap(0, 0, Logo_Bar_bits, 128, 16, WHITE);

    display.display();
}
#endif
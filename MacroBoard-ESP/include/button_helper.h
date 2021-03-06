#include <BfButtonManager.h>

uint16_t reading, avg;
uint16_t sum = 0;

const unsigned int pin = 32;

void setup_button_helper()
{
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println();
}

void loop_button_helper()
{
    static unsigned int i = 0;
    reading = BfButtonManager::printReading(pin);
    if (reading > 100)
    { // button pressed
        sum += reading;
        if (i == 4)
        {
            avg = sum / 5;
            Serial.print("Avarage Reading: ");
            Serial.println(avg);
            sum = 0;
        }
        i++;
        if (i > 4)
            i = 0;
    }
    else
    { // button released
        sum = 0;
        i = 0;
    }
    delay(200);
}
#include "Adafruit_SHTC3.h"
#include <LiquidCrystal_I2C.h>

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{

    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd.clear();

    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("SHTC3 test");
    if (!shtc3.begin())
    {
        Serial.println("Couldn't find SHTC3");
        while (1)
            delay(1);
    }
    Serial.println("Found SHTC3 sensor");
}

void loop()
{
    sensors_event_t humidity, temp;

    shtc3.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data

    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println("% rH");

    lcd.clear();
    lcd.print(temp.temperature);
    lcd.print(" degrees C");
    lcd.setCursor(0, 1);
    lcd.print(humidity.relative_humidity);
    lcd.print("% rH");

    delay(2000);
}
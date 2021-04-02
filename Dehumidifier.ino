#include <RotaryEncoder.h>
#include "Adafruit_SHTC3.h"
#include <LiquidCrystal_I2C.h>
#include <EasyButton.h>
#include <EEPROM.h>

#define btn_pin A3
#define enc_a_pin A6
#define enc_b_pin A7
#define relay_pin A2
#define update_time 1000

#define ENCODER_CW RotaryEncoder::Direction::CLOCKWISE
#define ENCODER_CCW RotaryEncoder::Direction::COUNTERCLOCKWISE
#define ENCODER_NA RotaryEncoder::Direction::NOROTATION

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
LiquidCrystal_I2C lcd(0x27, 16, 2);
RotaryEncoder encoder(enc_a_pin, enc_b_pin);
EasyButton btn(btn_pin);

bool running;
bool off;
int target;
unsigned long time;

void setup()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
    shtc3.begin();
    btn.begin();
    pinMode(enc_a_pin, INPUT);
    pinMode(enc_b_pin, INPUT);
    pinMode(relay_pin, OUTPUT);
    running = false;
    off = false;
    target = 80;
    time = millis() + update_time; // to read sensors on boot

    //debug
    Serial.begin(9600);
}

void loop()
{
    // read user inputs
    encoder.tick();
    btn.read();

    // handle inputs
    RotaryEncoder::Direction dir = encoder.getDirection();
    if(dir == ENCODER_CW && target < 90)
    {
        target++;
        Serial.println("CW");
    }
    else if(dir == ENCODER_CCW && target > 30)
    {
        target--;
        Serial.println("CCW");
    }

    if(btn.wasPressed())
    {
        off = !off;
        Serial.println("btn");
    }
    

    // read sensor
    if (millis() - time > update_time)
    {
        sensors_event_t humidity, temp;
        shtc3.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
        // print result
        lcd.setCursor(0, 0);
        lcd.print("Now:");
        lcd.print(humidity.relative_humidity, 1);
        lcd.setCursor(0, 1);
        lcd.print(temp.temperature, 1);
        lcd.print(" C ");
        // turn on or off relay
        if(humidity.relative_humidity > target)
            running = true;
        else if(humidity.relative_humidity < target - 1) // leave a margin to avoid frequent on off
            running = false;

        time = millis();
    }
    // print target
    lcd.setCursor(8, 0);
    lcd.print("% Set:");
    lcd.print(target);
    lcd.print("%");

    // print status
    lcd.setCursor(7, 1);
    if (off)
        lcd.print("[ STOP ] ");
    else if (running)
        lcd.print("[RUNNING]");
    else
        lcd.print("[STANDBY]");

    // set relay
    if(!off && running)
        digitalWrite(relay_pin, HIGH);
    else
        digitalWrite(relay_pin, LOW);
}
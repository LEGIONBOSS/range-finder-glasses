/*
    File:    range-finder-glasses.ino
    Created: 2026.03.10.
    Author:  Daniel Szilagyi

    Measure distance from sonar
    Display meters on 7seg display (1 digit, 1 decimal)
*/

#include "src/HCSR04/HCSR04.h"                             // https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib
#include "src/ShiftRegister74HC595/ShiftRegister74HC595.h" // https://github.com/Simsso/ShiftRegister74HC595

#define PIN_SONAR_TRIG (PB3)     // HC-SR04 trigger pin
#define PIN_SONAR_ECHO (PB4)     // HC-SR04 echo pin
#define PIN_SHIFTREG_DATA (PB2)  // 74HC595 data pin
#define PIN_SHIFTREG_CLOCK (PB1) // 74HC595 clock pin
#define PIN_SHIFTREG_LATCH (PB0) // 74HC595 latch pin

HCSR04 sonar(PIN_SONAR_TRIG, PIN_SONAR_ECHO);
ShiftRegister74HC595<2> sreg(PIN_SHIFTREG_DATA, PIN_SHIFTREG_CLOCK, PIN_SHIFTREG_LATCH);

static uint8_t digit_lut[10]
{
    // https://robojax.com/RJT582
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};

static uint8_t add_decimal_point(uint8_t digit)
{
    return digit &= ~(1 << 7);
}

// [whole digit][decimal digit]
static uint16_t dist_to_byte(float dist_cm)
{
    static uint8_t num1 = (uint8_t)(dist_cm + 5) / 100;
    static uint8_t digit1 = digit_lut[num1] &= ~(1 << 7);

    static uint8_t num01 = ((uint8_t)(dist_cm + 5) / 10) % 10;
    static uint8_t digit01 = digit_lut[num01];

    return (((uint16_t)digit1) << 8) + (uint16_t)digit01;
}

void setup(void) {};

void loop(void)
{
    // Wait 1s
    delay(1000);

    // Get distance
    static float dist_cm = sonar.dist();

    // Set display
    static uint16_t digits = dist_to_byte(dist_cm);
    static uint8_t pin_values[2];
    pin_values[0] = (uint8_t)(digits);      // LSB byte, right digit
    pin_values[1] = (uint8_t)(digits >> 8); // MSB byte, left digit
    sreg.setAll(pin_values);
}

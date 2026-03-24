/*
    File:    range-finder-glasses.ino
    Created: 2026.03.10.
    Author:  Daniel Szilagyi

    Measure distance from sonar
    Display meters on 7seg display (1 digit, 1 decimal)
*/

#include "src/HCSR04/HCSR04.h"                             // https://github.com/Martinsos/arduino-lib-hc-sr04
#include "src/ShiftRegister74HC595/ShiftRegister74HC595.h" // https://github.com/Simsso/ShiftRegister74HC595

#define SONAR_PIN_TRIG (A3)     // HC-SR04 trigger pin
#define SONAR_PIN_ECHO (A2)     // HC-SR04 echo pin
#define SHIFTREG_PIN_DATA (A1)  // 74HC595 data pin
#define SHIFTREG_PIN_CLOCK (A0) // 74HC595 clock pin
#define SHIFTREG_PIN_LATCH (13) // 74HC595 latch pin

UltraSonicDistanceSensor sonar(SONAR_PIN_TRIG, SONAR_PIN_ECHO);
ShiftRegister74HC595<2> sreg(SHIFTREG_PIN_DATA, SHIFTREG_PIN_CLOCK, SHIFTREG_PIN_LATCH);

static const uint8_t _digit_lut[10]
{               //           0
    0b11000000, // 0      -------
    0b11111001, // 1     |       |
    0b10100100, // 2   5 |       | 1
    0b10110000, // 3     |   6   |
    0b10011001, // 4      -------
    0b10010010, // 5     |       |
    0b10000010, // 6   4 |       | 2
    0b11111000, // 7     |       |
    0b10000000, // 8      -------  O
    0b10010000  // 9         3     7
};

// whole digit -> high byte, decimal digit -> low byte
static uint16_t dist_to_byte(float dist_cm)
{
    // Cast to uint16_t first, otherwise it overflows after 255cm
    uint8_t num1 = (uint16_t)(dist_cm + 5) / 100;
    uint8_t digit1 = _digit_lut[num1];
    digit1 &= ~(1 << 7); // Add decimal point

    uint8_t num01 = ((uint16_t)(dist_cm + 5) / 10) % 10;
    uint8_t digit01 = _digit_lut[num01];

    return (((uint16_t)digit1) << 8) + (uint16_t)digit01;
}

void setup(void) {}

void loop(void)
{
    // Wait 1s
    delay(1000);

    // Get distance
    float dist_cm = sonar.measureDistanceCm();
    if (dist_cm == -1) dist_cm = 0; // Ignore library error

    // Set display
    uint16_t digits = dist_to_byte(dist_cm);
    uint8_t pin_values[2];
    pin_values[0] = (uint8_t)(digits >> 8); // Low byte, left digit
    pin_values[1] = (uint8_t)(digits);      // High byte, right digit
    sreg.setAll(pin_values);
}

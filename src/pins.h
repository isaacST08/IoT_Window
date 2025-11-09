#ifndef PINS_H
#define PINS_H

#define RED_LED_PIN 0
#define GREEN_LED_PIN 3  // Removed
#define YELLOW_LED_PIN 1
#define BLUE_LED_PIN 2

// Stepper Motor Pins
#define SM_ENABLE_PIN 17  // Stepper Motor Enable Pin
#define SM_DIR_PIN 20     // Stepper Motor Direction Pin
#define SM_PULSE_PIN 18   // Stepper Motor Pulse Pin
#define SM_MS1_PIN 16     // Stepper Motor Micro-Step Pin A
#define SM_MS2_PIN 19     // Stepper Motor Micro-Step Pin B

// Limit Switch Pins
#define LS_1 12  // Limit switch 1
#define LS_2 13  // Limit switch 2

// Micro-Step Configurations (Low bit for MS1, high bit for MS2).
#define MS_8 0b00
#define MS_16 0b11
#define MS_32 0b01
#define MS_64 0b10

void init_pins();

#endif

#ifndef PINS_HH
#define PINS_HH

// #include <hardware/gpio.h>
// #include <pico/types.h>
// #include <stdbool.h>

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

#define MS_ENCODE(ms_int)   \
  ((ms_int == 8)    ? MS_8  \
   : (ms_int == 16) ? MS_16 \
   : (ms_int == 32) ? MS_32 \
   : (ms_int == 64) ? MS_64 \
                    : MS_64)

#define MS_DECODE(ms_pins)   \
  ((ms_pins == MS_8)    ? 8  \
   : (ms_pins == MS_16) ? 16 \
   : (ms_pins == MS_32) ? 32 \
   : (ms_pins == MS_64) ? 64 \
                        : 1)

void init_pins();
// void init_pin(uint pin, bool dir, bool init_val);

#endif

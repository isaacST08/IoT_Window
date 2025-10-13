#include "pins.h"

#include <hardware/gpio.h>

/**
 * Initializes the pins for the system.
 *
 * \param initial_micro_step The initial micro-step state for the system. The
 *   low order bit is the state for MS1, the second bit is the state for MS2,
 *   further bits are ignored.
 */
void init_pins(int initial_micro_step) {
  // Red LED Pin.
  gpio_init(RED_LED_PIN);
  gpio_set_dir(RED_LED_PIN, GPIO_OUT);

  // Stepper Motor Enable Pin.
  gpio_init(SM_ENABLE_PIN);
  gpio_set_dir(SM_ENABLE_PIN, GPIO_OUT);

  // Stepper Motor Direction Pin.
  gpio_init(SM_DIR_PIN);
  gpio_set_dir(SM_DIR_PIN, GPIO_OUT);

  // Stepper Motor Pulse Pin.
  gpio_init(SM_PULSE_PIN);
  gpio_set_dir(SM_PULSE_PIN, GPIO_OUT);

  // Stepper Motor Micro-Step Pin A.
  gpio_init(SM_MS1_PIN);
  gpio_set_dir(SM_MS1_PIN, GPIO_OUT);
  gpio_put(SM_MS1_PIN, initial_micro_step & 0b1);

  // Stepper Motor Micro-Step Pin B.
  gpio_init(SM_MS2_PIN);
  gpio_set_dir(SM_MS2_PIN, GPIO_OUT);
  gpio_put(SM_MS2_PIN, (initial_micro_step >> 1) & 0b1);
}

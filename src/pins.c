#include "pins.h"

#include <hardware/gpio.h>

#define INIT_LED_PIN(pin)        \
  ({                             \
    gpio_init(pin);              \
    gpio_set_dir(pin, GPIO_OUT); \
    gpio_put(pin, 0);            \
  })

/**
 * Initializes the pins for the system.
 *
 * \param initial_micro_step The initial micro-step state for the system. The
 *   low order bit is the state for MS1, the second bit is the state for MS2,
 *   further bits are ignored.
 */
void init_pins() {
  // LEDs
  INIT_LED_PIN(RED_LED_PIN);
  INIT_LED_PIN(GREEN_LED_PIN);
  INIT_LED_PIN(YELLOW_LED_PIN);
  INIT_LED_PIN(BLUE_LED_PIN);

  // Limit Switches
  gpio_init(LS_1);
  gpio_init(LS_2);
  gpio_set_pulls(LS_1, true, false);  // Pull up
  gpio_set_pulls(LS_2, true, false);  // Pull up
}

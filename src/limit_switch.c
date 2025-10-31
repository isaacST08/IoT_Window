#include "limit_switch.h"

#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdbool.h>

void lsInit(LimitSwitch* ls, uint trigger_pin, bool triggered_condition,
            bool pull_up) {
  // Set limit switch object variables.
  ls->triggered_condition = triggered_condition;
  // Set pins.
  ls->trigger_pin = trigger_pin;

  // Initialize pins.
  gpio_init(ls->trigger_pin);
  gpio_set_dir(ls->trigger_pin, GPIO_IN);
  if (pull_up)
    gpio_set_pulls(ls->trigger_pin, true, false);  // Pull up.
  else
    gpio_set_pulls(ls->trigger_pin, false, true);  // Pull down.
}

bool lsTriggered(LimitSwitch* ls) {
  return (gpio_get(ls->trigger_pin) == ls->triggered_condition);
}

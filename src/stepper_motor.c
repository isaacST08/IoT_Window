#include "stepper_motor.h"

#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>

#include "pins.h"

/**
 * Enables the stepper motor.
 */
void sm_enable() { gpio_put(SM_ENABLE_PIN, 0); }

/**
 * Disables the stepper motor.
 */
void sm_disable() { gpio_put(SM_ENABLE_PIN, 1); }

/**
 * Performs one step for the stepper motor.
 *
 * \param half_delay_us Half of the delay for the step in microseconds.
 *   This delay happens once after the pin is set true, and again after
 *   the pin is set false.
 */
void sm_step(uint64_t half_delay_us) {
  gpio_put(SM_PULSE_PIN, 1);
  sleep_us(half_delay_us);
  gpio_put(SM_PULSE_PIN, 0);
  sleep_us(half_delay_us);
}

/**
 * Returns the current micro-step state.
 * Compare against one of MS_8, MS_16, MS_32, MS_64 for a human readable value.
 *
 * \returns The current micro-step state in it's binary pin form.
 */
int get_micro_step() {
  return ((gpio_get(SM_MS2_PIN) << 1) & gpio_get(SM_MS1_PIN));
}

#include "stepper_motor.h"

#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "advanced_opts.h"
#include "common.h"
#include "opts.h"
#include "pins.h"

/**
 * Initialize a stepper motor object.
 *
 * \param sm The stepper motor to initialize.
 * \param enable_pin The enable pin of the motor.
 * \param direction_pin The direction pin of the motor.
 * \param pulse_pin The pulse/step pin of the motor.
 * \param micro_step_1_pin The first of the two micro step encoding pins for the
 * motor.
 * \param micro_step_2_pin The second of the two micro step encoding pins for
 * the motor.
 * \param initial_micro_step The initial micros step to set the motor to.
 * \param initial_speed The initial speed to set the motor to.
 */
void smInit(StepperMotor* sm, uint enable_pin, uint direction_pin,
            uint pulse_pin, uint micro_step_1_pin, uint micro_step_2_pin,
            uint initial_micro_step, uint8_t initial_speed) {
  // Set stepper motor pins.
  sm->pins.enable = enable_pin;
  sm->pins.direction = direction_pin;
  sm->pins.pulse = pulse_pin;
  sm->pins.ms1 = micro_step_1_pin;
  sm->pins.ms2 = micro_step_2_pin;

  // ----- Initialize the pins -----
  // Enable Pin.
  gpio_init(sm->pins.enable);
  gpio_set_dir(sm->pins.enable, GPIO_OUT);
  gpio_put(sm->pins.enable, 0);

  // Motor Direction Pin.
  gpio_init(sm->pins.direction);
  gpio_set_dir(sm->pins.direction, GPIO_OUT);
  gpio_put(sm->pins.direction, 0);

  // Motor Pulse Pin.
  gpio_init(sm->pins.pulse);
  gpio_set_dir(sm->pins.pulse, GPIO_OUT);
  gpio_put(sm->pins.pulse, 0);

  // Stepper Motor Micro-Step Pin A.
  gpio_init(sm->pins.ms1);
  gpio_set_dir(sm->pins.ms1, GPIO_OUT);

  // Stepper Motor Micro-Step Pin B.
  gpio_init(sm->pins.ms2);
  gpio_set_dir(sm->pins.ms2, GPIO_OUT);

  // ----- Set Initial Values -----
  // Quiet mode off by default.
  sm->quiet_mode = false;

  // No call to stop the motor.
  sm->stop_motor = false;

  // Zero the position (assume).
  sm->step_position = 0;

  // No queued actions yet.
  sm->queued_action = NONE;

  // Set the initial micro steps value of the motor.
  smSetMicroStep(sm, initial_micro_step);

  // Default motor speed.
  smSetSpeed(sm, initial_speed);
}

/**
 * Returns the current micro-step pin encoded state.
 *
 * Compare against one of MS_8, MS_16, MS_32, MS_64 for a human readable value.
 *
 * \returns The current micro-step state in it's binary pin form.
 */
uint smGetMicroStep(StepperMotor* sm) {
  return ((gpio_get(sm->pins.ms2) << 1) & gpio_get(sm->pins.ms1));
}

/**
 * Returns the current micro-steps of the motor as an int.
 *
 * \returns The current micro-step state in it's integer form.
 */
uint smGetMicroStepInt(StepperMotor* sm) {
  int micro_steps = smGetMicroStep(sm);
  switch (micro_steps) {
    case MS_8:
      return 8;
      break;
    case MS_16:
      return 16;
      break;
    case MS_32:
      return 32;
      break;
    case MS_64:
      return 64;
      break;
  }
  return 1;
}

/**
 * Set the micro steps for the stepper motor.
 *
 * \param sm The stepper motor to set the micro steps of.
 * \param micro_step The micro steps to set the motor to.
 */
void smSetMicroStep(StepperMotor* sm, uint micro_step) {
  // Record the current state of the stepper motor.
  uint current_ms_int = smGetMicroStepInt(sm);
  bool saved_dir = smGetDir(sm);

  // Determine the integer form of the desired micro step and
  // insure that the desired ms is indeed an accepted value.
  uint desired_ms_int;
  uint desired_ms = micro_step;
  switch (micro_step) {
    case MS_8:
      desired_ms_int = 8;
      break;
    case MS_16:
      desired_ms_int = 16;
      break;
    case MS_32:
      desired_ms_int = 32;
      break;
    case MS_64:
      desired_ms_int = 64;
      break;
    default:
      desired_ms_int = 64;
      desired_ms = MS_64;
  }

  // Align the steps to a multiple of the new micro step if the new micro step
  // has less resolution than the current micro step.
  if (desired_ms_int < current_ms_int) {
    // Calculate how many steps are required to align to the new micro steps.
    int64_t required_steps =
        sm->step_position % (SM_SMALLEST_MS / desired_ms_int);

    // Make the required steps as small as possible and absolute.
    // We will step towards the zero position.
    if (sm->step_position < 0) {
      required_steps -= (SM_SMALLEST_MS / desired_ms_int);
    }

    // Set the direction of the motor to step towards the zero point position.
    // Dir of 0 goes up, 1 goes down. So if the position is positive, that means
    // we set the direction to be 1 and to go down (towards zero).
    smSetDir(sm, (sm->step_position >= 0));

    // Perform the steps.
    for (int i = 0; i < required_steps; i++) {
      smStep(sm);
    }

    // Return the motor direction to how it was found.
    smSetDir(sm, saved_dir);
  }

  // Set the micro step pins for the new micro step value.
  gpio_put(sm->pins.ms1, desired_ms & 0b1);
  gpio_put(sm->pins.ms2, (desired_ms >> 1) & 0b1);
}

/**
 * Enables the stepper motor.
 */
void smEnable(StepperMotor* sm) { gpio_put(sm->pins.enable, 0); }

/**
 * Disables the stepper motor.
 */
void smDisable(StepperMotor* sm) { gpio_put(sm->pins.enable, 1); }

/**
 * Set the direction of the stepper motor.
 *
 * \param sm The stepper motor to set the direction of.
 * \param dir The direction to set.
 *            0 is clockwise, 1 is counter clockwise.
 */
void smSetDir(StepperMotor* sm, bool dir) { gpio_put(sm->pins.direction, dir); }

/**
 * Gets the current direction of the stepper motor.
 *
 * \param sm The stepper motor to get the direction for.
 *
 * \returns The current direction of the motor.
 *          0 is clockwise, 1 is counter clockwise.
 */
bool smGetDir(StepperMotor* sm) { gpio_get(sm->pins.direction); }

/**
 * Swaps the current direction of the stepper motor.
 *
 * \param sm The stepper motor to swap the direction for.
 */
void smSwapDir(StepperMotor* sm) { smSetDir(sm, smGetDir(sm) ^ 1); }

/**
 * Set the speed of the stepper motor.
 *
 * This speed value is inverted: lower values result in faster motor speeds.
 *
 * \param sm The stepper motor to set the speed of.
 * \param speed The speed to set the motor to.
 */
void smSetSpeed(StepperMotor* sm, uint8_t speed) {
  sm->speed = speed;
  sm->half_step_delay = speed * MIN_US_PER_HALF_SMALLEST_MS +
                        ((sm->quiet_mode) ? SM_QUIET_MODE_ADDITIONAL_US : 0);
}

/**
 * Gets the current speed of the stepper motor.
 *
 * This speed value is inverted: lower values result in faster motor speeds.
 * The speed value directly relates to the half step speed of the motor in micro
 * seconds (assuming that quiet mode is not set).
 *
 * \param sm The stepper motor to get the speed of.
 *
 * \returns The speed the motor is set to. This value relates to half of the
 * delay per micro step in micro seconds.
 */
uint64_t smGetSpeed(StepperMotor* sm) { return sm->speed; }

/**
 * Sets the quiet mode for the motor.
 *
 * \param mode Whether to enable or disable quiet mode.
 */
void smSetQuietMode(StepperMotor* sm, bool mode) {
  // Set the quiet mode flag.
  sm->quiet_mode = mode;

  // Update the motor speed.
  smSetSpeed(sm, sm->speed);
}

/**
 * Get the current position of the motor in increments of the smallest micro
 * step (step distance largest, not integer largest. So 64MS < 16MS.).
 */
uint64_t smGetPosition(StepperMotor* sm) { return sm->step_position; }

/**
 * Performs one step of the motor with a total step time of double the
 * `half_step_delay` value in micro seconds.
 *
 * Records the position change at the end of the step.
 *
 * \param sm The stepper motor to step.
 * \param half_step_delay The half value for the total time the step will take.
 */
void smStepExact(StepperMotor* sm, uint64_t half_step_delay) {
  // Perform the step.
  gpio_put(sm->pins.pulse, 1);
  sleep_us(half_step_delay);
  gpio_put(sm->pins.pulse, 0);
  sleep_us(half_step_delay);

  // Record the position change.

  // Option A:
  if (smGetDir(sm)) {
    sm->step_position -= SM_SMALLEST_MS / smGetMicroStepInt(sm);
  } else
    sm->step_position += SM_SMALLEST_MS / smGetMicroStepInt(sm);

  // // Option B:
  // sm->step_position +=
  //     (1 - 2 * smGetDir(sm)) * (SM_SMALLEST_MS / smGetMicroStepInt(sm));
}

/**
 * Performs one step of the motor.
 *
 * \param sm The stepper motor to step.
 */
void smStep(StepperMotor* sm) { smStepExact(sm, sm->half_step_delay); }

/**
 * Homes the stepper motor to find the zero position.
 */
void smHome(StepperMotor* sm) {
  // Save current the state of the motor.
  bool saved_dir = smGetDir(sm);
  uint saved_ms = smGetMicroStep(sm);
  uint8_t saved_speed = smGetSpeed(sm);

  // Set the motor to move in the home direction.
  smSetDir(sm, HOME_DIR);

  // Set the micro steps to the most precise.
  smSetMicroStep(sm, MS_64);

  // Perform the first home.
  smSetSpeed(sm, 2);
  while (gpio_get(LS_HOME) != 1) smStep(sm);

  // Move back some
  smSetDir(sm, HOME_DIR ^ 1);
  smSetSpeed(sm, 1);
  while (gpio_get(LS_HOME) == 1) smStep(sm);
  for (int i = 0; i < 2000; i++) smStep(sm);

  // Perform the second, slower, home.
  smSetDir(sm, HOME_DIR);
  smSetSpeed(sm, 4);
  while (gpio_get(LS_HOME) != 1) smStep(sm);

  // Update the zero position of the motor.
  sm->step_position = 0;

  // Restore the motor settings.
  smSetDir(sm, saved_dir);
  smSetMicroStep(sm, saved_ms);
  smSetSpeed(sm, saved_speed);
}

/**
 * Closes the stepper motor when controlling a window.
 *
 * \returns Whether the operation was successful and the motor was not told to
 * stop part way.
 */
bool smClose(StepperMotor* sm) {
  // Set the motor to move in the closing direction.
  smSetDir(sm, CLOSE_DIR);

  // Reset any call to stop the motor.
  sm->stop_motor = false;

  // Move the motor in the close direction until either:
  // - The motor is told to stop, or
  // - The end stop is found, or
  // - The encoded window position in steps reaches the expected closed
  //   position.
  while (!sm->stop_motor && gpio_get(LS_CLOSED) != 1 &&
         sm->step_position != WINDOW_CLOSED_STEP_POSITION)
    smStep(sm);

  // Return true if the motor successfully closed the window and was not called
  // to stop.
  return !sm->stop_motor;
}

/**
 * Opens the stepper motor when controlling a window.
 *
 * \returns Whether the operation was successful and the motor was not told to
 * stop part way.
 */
bool smOpen(StepperMotor* sm) {
  // Set the motor to move in the opening direction.
  smSetDir(sm, OPEN_DIR);

  // Reset any call to stop the motor.
  sm->stop_motor = false;

  // Move the motor in the open direction until either:
  // - The motor is told to stop, or
  // - The end stop is found, or
  // - The encoded window position in steps reaches the expected open position.
  while (!sm->stop_motor && gpio_get(LS_OPEN) != 1 &&
         sm->step_position != WINDOW_OPEN_STEP_POSITION)
    smStep(sm);

  // Return true if the motor successfully opened the window and was not called
  // to stop.
  return !sm->stop_motor;
}

/**
 * Sets the flag to stop the motor.
 *
 * It is up to the other functions to respect this flag.
 */
void smStop(StepperMotor* sm) { sm->stop_motor = true; }

// // ----- OLD -----
//
// static bool stop_motor = false;
// static uint64_t* motor_half_delay;
//
// /**
//  * Enables the stepper motor.
//  */
// void sm_enable() { gpio_put(SM_ENABLE_PIN, 0); }
//
// /**
//  * Disables the stepper motor.
//  */
// void sm_disable() { gpio_put(SM_ENABLE_PIN, 1); }
//
// void sm_set_speed_ptr(uint64_t* speed) { motor_half_delay = speed; }
//
// /**
//  * Performs one step for the stepper motor.
//  *
//  * \param half_delay_us Half of the delay for the step in microseconds.
//  *   This delay happens once after the pin is set true, and again after
//  *   the pin is set false.
//  */
// void sm_step(uint64_t half_delay_us) {
//   gpio_put(SM_PULSE_PIN, 1);
//   sleep_us(half_delay_us);
//   gpio_put(SM_PULSE_PIN, 0);
//   sleep_us(half_delay_us);
// }
//
// void sm_set_dir(int dir) { gpio_put(SM_DIR_PIN, dir); }
//
// /**
//  * Returns the current micro-step state.
//  * Compare against one of MS_8, MS_16, MS_32, MS_64 for a human readable
//  value.
//  *
//  * \returns The current micro-step state in it's binary pin form.
//  */
// int get_micro_step() {
//   return ((gpio_get(SM_MS2_PIN) << 1) & gpio_get(SM_MS1_PIN));
// }
//
// void sm_home() {
//   sm_set_dir(HOME_DIR);
//
//   int home_ls_state = gpio_get(LS_HOME);
//   printf("Home LS pin state: %d\r", home_ls_state);
//
//   int steps_completed = 0;
//   while (home_ls_state != 1) {
//     sm_step(*motor_half_delay);
//     steps_completed++;
//     home_ls_state = gpio_get(LS_HOME);
//     printf("Home LS pin state: %d, Steps completed: %d\r", home_ls_state,
//            steps_completed);
//   }
//   printf("\nHomeing Completed. Moving back\n");
//
//   sm_set_dir(HOME_DIR ^ 1);
//
//   for (int i = 0; i < 2000; i++) {
//     sm_step(*motor_half_delay);
//   }
// }
//
// bool sm_close() {
//   // printf("Closing window...\n");
//   sm_set_dir(CLOSE_DIR);
//
//   // int home_ls_state = gpio_get(LS_CLOSED);
//
//   stop_motor = false;
//   // int steps_completed = 0;
//   while (gpio_get(LS_CLOSED) != 1 && !stop_motor) {
//     sm_step(*motor_half_delay);
//     // steps_completed++;
//     // home_ls_state = gpio_get(LS_CLOSED);
//   }
//   // printf("Window closed.\n");
//
//   return !stop_motor;
// }
//
// bool sm_open() {
//   sm_set_dir(OPEN_DIR);
//
//   stop_motor = false;
//   while (gpio_get(LS_OPEN) != 1 && !stop_motor) {
//     sm_step(*motor_half_delay);
//   }
//
//   return !stop_motor;
// }
//
// void sm_stop() {
//   stop_motor = true;
//   // sleep_ms(5000);
//   // stop_motor = false;
// }

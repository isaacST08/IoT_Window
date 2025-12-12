#include "stepper_motor.hh"

#include <hardware/gpio.h>
#include <math.h>
#include <pico/platform/compiler.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "advanced_opts.h"
#include "common.h"
#include "limit_switch.h"
#include "opts.h"

extern "C" {
#include "pins.h"
}

using namespace stepper_motor;

#define MM_PER_SEC_TO_US_PER_HALF_MICROSTEP(mm_per_sec, micro_step) \
  ceil(1000000.0 / (mm_per_sec * SM_FULL_STEPS_PER_MM * micro_step * 2))

/**
 * Initialize a stepper motor object.
 *
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
stepper_motor::StepperMotor::StepperMotor(uint enable_pin, uint direction_pin,
                                          uint pulse_pin, uint micro_step_1_pin,
                                          uint micro_step_2_pin,
                                          uint initial_micro_step,
                                          float initial_speed) {
  // Set stepper motor pins.
  this->pins.enable = enable_pin;
  this->pins.direction = direction_pin;
  this->pins.pulse = pulse_pin;
  this->pins.ms1 = micro_step_1_pin;
  this->pins.ms2 = micro_step_2_pin;

  // ----- Initialize the pins -----
  // Enable Pin.
  gpio_init(this->pins.enable);
  gpio_set_dir(this->pins.enable, GPIO_OUT);
  gpio_put(this->pins.enable, 0);

  // Motor Direction Pin.
  gpio_init(this->pins.direction);
  gpio_set_dir(this->pins.direction, GPIO_OUT);
  gpio_put(this->pins.direction, 0);

  // Motor Pulse Pin.
  gpio_init(this->pins.pulse);
  gpio_set_dir(this->pins.pulse, GPIO_OUT);
  gpio_put(this->pins.pulse, 0);

  // Stepper Motor Micro-Step Pin A.
  gpio_init(this->pins.ms1);
  gpio_set_dir(this->pins.ms1, GPIO_OUT);

  // Stepper Motor Micro-Step Pin B.
  gpio_init(this->pins.ms2);
  gpio_set_dir(this->pins.ms2, GPIO_OUT);

  // ----- Set Initial Values -----
  // Quiet mode off by default.
  this->quiet_mode = false;

  // No call to stop the motor.
  this->stop_motor = false;

  // Zero the position (assume).
  this->step_position = 0;

  // No queued actions yet.
  this->queued_action = Action::NONE;

  // Not moving.
  this->state = State::STOPPED;

  // Set the initial micro steps value of the motor.
  this->setMicroStep(initial_micro_step);

  // Default motor speed.
  this->setSpeed(initial_speed);
}

/**
 * Returns the current micro-step pin encoded state.
 *
 * Compare against one of MS_8, MS_16, MS_32, MS_64 for a human readable value.
 *
 * \returns The current micro-step state in it's binary pin form.
 */
uint StepperMotor::getMicroStep() {
  return (((uint)gpio_get(this->pins.ms2) << 1) |
          (uint)gpio_get(this->pins.ms1));
}

/**
 * Returns the current micro-steps of the motor as an int.
 *
 * \returns The current micro-step state in it's integer form.
 */
uint StepperMotor::getMicroStepInt() {
  int micro_steps = this->getMicroStep();
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
 * \param micro_step The micro steps to set the motor to.
 */
void StepperMotor::setMicroStep(uint micro_step) {
  // Record the current state of the stepper motor.
  uint current_ms_int = this->getMicroStepInt();
  bool saved_dir = this->getDir();

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
        this->step_position % (SM_SMALLEST_MS / desired_ms_int);

    // Make the required steps as small as possible and absolute.
    // We will step towards the zero position.
    if (this->step_position < 0) {
      required_steps -= (SM_SMALLEST_MS / desired_ms_int);
    }

    // Set the direction of the motor to step towards the zero point position.
    // Dir of 0 goes up, 1 goes down. So if the position is positive, that means
    // we set the direction to be 1 and to go down (towards zero).
    StepperMotor::setDir(this->step_position >= 0);

    // Perform the steps.
    for (int i = 0; i < required_steps; i++) {
      this->step();
    }

    // Return the motor direction to how it was found.
    this->setDir(saved_dir);
  }

  // Set the micro step pins for the new micro step value.
  gpio_put(this->pins.ms1, desired_ms & 0b1);
  gpio_put(this->pins.ms2, (desired_ms >> 1) & 0b1);
}

/**
 * Enables the stepper motor.
 */
void StepperMotor::enable() { gpio_put(this->pins.enable, 0); }

/**
 * Disables the stepper motor.
 */
void StepperMotor::disable() { gpio_put(this->pins.enable, 1); }

/**
 * Set the direction of the stepper motor.
 *
 * \param dir The direction to set.
 *            0 is clockwise, 1 is counter clockwise.
 */
void StepperMotor::setDir(direction_t dir) {
  gpio_put(this->pins.direction, dir);
}

/**
 * Gets the current direction of the stepper motor.
 *
 * \returns The current direction of the motor.
 *          0 is clockwise, 1 is counter clockwise.
 */
direction_t StepperMotor::getDir() { return gpio_get(this->pins.direction); }

/**
 * Swaps the current direction of the stepper motor.
 */
void StepperMotor::swapDir() { this->setDir(this->getDir() ^ 1); }

/**
 * Set the speed of the stepper motor.
 *
 * \param speed The speed to set the motor to in millimeters per second.
 */
void StepperMotor::setSpeed(float speed) {
  this->speed = speed;
  // sm->half_step_delay = speed * MIN_US_PER_HALF_SMALLEST_MS +
  //                       ((sm->quiet_mode) ? SM_QUIET_MODE_ADDITIONAL_US : 0);
  // sm->half_step_delay = (speed + sm->quiet_mode * 4) *
  //                       MIN_US_PER_HALF_SMALLEST_MS *
  //                       (SM_SMALLEST_MS / smGetMicroStepInt(sm));

  // sm->half_step_delay = SM_FULL_STEPS_PER_MM * smGetMicroStepInt(sm)
  // sm->half_step_delay = ceil(1000000.0 / (speed * SM_FULL_STEPS_PER_MM * 2));

  if (this->quiet_mode) {
    uint64_t half_step_delay = MM_PER_SEC_TO_US_PER_HALF_MICROSTEP(speed, 64);
    this->setMicroStep(MS_64);
    this->half_step_delay = (SM_MS64_MIN_HALF_DELAY_QUIET < half_step_delay)
                                ? half_step_delay
                                : SM_MS64_MIN_HALF_DELAY_QUIET;
  } else {
    uint64_t possible_half_step_delay;
    uint64_t chosen_half_step_delay;
    uint chosen_micro_step_int;
    uint chosen_micro_step;

    // --- MS 64 ---
    possible_half_step_delay = MM_PER_SEC_TO_US_PER_HALF_MICROSTEP(speed, 64);
    if (SM_MS64_MIN_HALF_DELAY < possible_half_step_delay) {
      chosen_half_step_delay = possible_half_step_delay;
      chosen_micro_step_int = 64;
      chosen_micro_step = MS_64;
    } else {
      chosen_half_step_delay = SM_MS64_MIN_HALF_DELAY;
      chosen_micro_step_int = 64;
      chosen_micro_step = MS_64;
    }

    // --- MS 32 ---
    possible_half_step_delay = MM_PER_SEC_TO_US_PER_HALF_MICROSTEP(speed, 32);
    if (SM_MS32_MIN_HALF_DELAY < possible_half_step_delay &&
        possible_half_step_delay <
            (chosen_half_step_delay - 1) * (chosen_micro_step_int / 32)) {
      chosen_half_step_delay = possible_half_step_delay;
      chosen_micro_step_int = 32;
      chosen_micro_step = MS_32;
    }

    // --- MS 16 ---
    possible_half_step_delay = MM_PER_SEC_TO_US_PER_HALF_MICROSTEP(speed, 16);
    if (SM_MS16_MIN_HALF_DELAY < possible_half_step_delay &&
        possible_half_step_delay <
            (chosen_half_step_delay - 1) * (chosen_micro_step_int / 16)) {
      chosen_half_step_delay = possible_half_step_delay;
      chosen_micro_step_int = 16;
      chosen_micro_step = MS_16;
    }

    // --- MS 8 ---
    possible_half_step_delay = MM_PER_SEC_TO_US_PER_HALF_MICROSTEP(speed, 8);
    if (SM_MS8_MIN_HALF_DELAY < possible_half_step_delay &&
        possible_half_step_delay <
            (chosen_half_step_delay - 1) * (chosen_micro_step_int / 8)) {
      chosen_half_step_delay = possible_half_step_delay;
      chosen_micro_step_int = 8;
      chosen_micro_step = MS_8;
    }

    // Set determined values.
    this->setMicroStep(chosen_micro_step);
    this->half_step_delay = chosen_half_step_delay;
  }
}

/**
 * Gets the current speed of the stepper motor.
 *
 * This speed value is inverted: lower values result in faster motor speeds.
 * The speed value directly relates to the half step speed of the motor in micro
 * seconds (assuming that quiet mode is not set).
 *
 * \returns The speed the motor is set to. This value relates to half of the
 * delay per micro step in micro seconds.
 */
float StepperMotor::getSpeed() { return this->speed; }

/**
 * Sets the quiet mode for the motor.
 *
 * \param mode Whether to enable or disable quiet mode.
 */
void StepperMotor::setQuietMode(bool mode) {
  // Set the quiet mode flag.
  this->quiet_mode = mode;

  // Update the motor speed.
  this->setSpeed(this->speed);
}

/**
 * Gets the current quiet mode of the motor.
 */
bool StepperMotor::getQuietMode() { return this->quiet_mode; }

/**
 * Get the current position of the motor in increments of the smallest micro
 * step (step distance largest, not integer largest. So 64MS < 16MS.).
 */
uint64_t StepperMotor::getPosition() { return this->step_position; }

/**
 * Get the current position of the motor in increments of the smallest micro
 * step (step distance largest, not integer largest. So 64MS < 16MS.).
 */
int StepperMotor::getPositionPercentage() {
  // printf("Motor steps: %lld\n", this->step_position);
  // printf("Window width mm: %d\n", WINDOW_WIDTH_MM);
  // printf("Window width full steps: %d\n",
  //        WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM);
  // printf("Window width micro steps: %d\n",
  //        WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS);
  // printf("Motor position percentage long: %llu\n",
  //        (100 * this->step_position) /
  //            (WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS));
  // printf("Motor position percentage int: %d\n",
  //        (int)(100 * this->step_position) /
  //            (WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS));
  // return ((100 * this->step_position) /
  //         (WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS));
  return (int)round(this->getPositionPercentageExact());
}

float StepperMotor::getPositionPercentageExact() {
  return this->stepsToPercentage(this->step_position);
}

/**
 * Converts a number of steps representing the number of steps that the window
 * is open to the percentage that the window is open.
 *
 * @param steps The number of steps to convert.
 * @return The percentage the number of steps represents.
 */
float StepperMotor::stepsToPercentage(uint64_t steps) {
  return ((100.0 * this->step_position) /
          (WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS));
};

/**
 * Converts a percentage representing the percentage that the window is open to
 * the number of steps that the window is open.
 *
 * @param percentage The percentage to convert.
 * @return The number of steps the percentage represents.
 */
uint64_t StepperMotor::percentageToSteps(float percentage) {
  return ((uint64_t)llround(
      percentage * (WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS) /
      100));
};

/**
 * Performs one step of the motor with a total step time of double the
 * `half_step_delay` value in micro seconds.
 *
 * Records the position change at the end of the step.
 *
 * \param half_step_delay The half value for the total time the step will take.
 */
void StepperMotor::stepExact(uint64_t half_step_delay) {
  // Perform the step.
  gpio_put(this->pins.pulse, 1);
  sleep_us(half_step_delay);
  gpio_put(this->pins.pulse, 0);
  sleep_us(half_step_delay);

  // Record the position change.

  // Option A:
  if (this->getDir()) {
    this->step_position -= SM_SMALLEST_MS / this->getMicroStepInt();
  } else
    this->step_position += SM_SMALLEST_MS / this->getMicroStepInt();

  // // Option B:
  // this->step_position +=
  //     (1 - 2 * this->getDir()) * (SM_SMALLEST_MS / this->getMicroStepInt());
}

/**
 * Performs one step of the motor.
 */
void StepperMotor::step() { this->stepExact(this->half_step_delay); }

/**
 * Homes the stepper motor to find the zero position.
 */
void StepperMotor::home() {
  // Save current the state of the motor.
  bool saved_dir = this->getDir();
  uint saved_ms = this->getMicroStep();
  uint8_t saved_speed = this->getSpeed();

  // Set the motor to move in the home direction.
  this->setDir(HOME_DIR);

  // Set the micro steps to the most precise.
  this->setMicroStep(MS_64);

  // Perform the first home.
  this->setSpeed(3);
  // while (gpio_get(LS_HOME) != 1) smStep(sm);
  while (!LS_TRIGGERED(LS_HOME)) this->step();

  // Move back some
  this->setDir(HOME_DIR ^ 1);
  this->setSpeed(3);
  while (LS_TRIGGERED(LS_HOME)) this->step();
  uint current_ms = this->getMicroStepInt();
  for (int i = 0; i < (SM_FULL_STEPS_PER_MM * current_ms * 7); i++)
    this->step();

  // Perform the second, slower, home.
  this->setDir(HOME_DIR);
  this->setSpeed(1);
  while (!LS_TRIGGERED(LS_HOME)) step();

  // Update the zero position of the motor.
  this->step_position = 0;

  // Restore the motor settings.
  this->setDir(saved_dir);
  this->setMicroStep(saved_ms);
  this->setSpeed(saved_speed);
}

/**
 * Closes the stepper motor when controlling a window.
 *
 * \returns Whether the operation was successful and the motor was not told to
 * stop part way.
 */
bool StepperMotor::close() {
  // Set the motor to move in the closing direction.
  this->setDir(CLOSE_DIR);

  // Reset any call to stop the motor.
  this->stop_motor = false;

  // Set the current state.
  this->state = State::CLOSING;

  // Move the motor in the close direction until either:
  // - The motor is told to stop, or
  // - The end stop is found, or
  // - The encoded window position in steps reaches the expected closed
  //   position.
  while (!this->stop_motor && !LS_TRIGGERED(LS_CLOSED) &&
         this->step_position != WINDOW_CLOSED_STEP_POSITION)
    this->step();

  // Update the motor state.
  this->state = (this->stop_motor) ? State::STOPPED : State::CLOSED;
  this->queued_action = Action::NONE;
  if (LS_TRIGGERED(LS_CLOSED))
    this->step_position = WINDOW_CLOSED_STEP_POSITION;

  // Return true if the motor successfully closed the window and was not called
  // to stop.
  return !this->stop_motor;
}

/**
 * Opens the stepper motor when controlling a window.
 *
 * \returns Whether the operation was successful and the motor was not told to
 * stop part way.
 */
bool StepperMotor::open() {
  // Set the motor to move in the opening direction.
  this->setDir(OPEN_DIR);

  // Reset any call to stop the motor.
  this->stop_motor = false;

  // Set the current state.
  this->state = State::OPENING;

  // Move the motor in the open direction until either:
  // - The motor is told to stop, or
  // - The end stop is found, or
  // - The encoded window position in steps reaches the expected open position.
  while (!this->stop_motor && !LS_TRIGGERED(LS_OPEN) &&
         this->step_position != WINDOW_OPEN_STEP_POSITION)
    this->step();

  // Update the motor state.
  this->state = (this->stop_motor) ? State::STOPPED : State::OPEN;
  this->queued_action = Action::NONE;
  if (LS_TRIGGERED(LS_OPEN)) this->step_position = WINDOW_CLOSED_STEP_POSITION;

  // Return true if the motor successfully opened the window and was not called
  // to stop.
  return !this->stop_motor;
}

/**
 * Sets the flag to stop the motor.
 *
 * It is up to the other functions to respect this flag.
 */
void StepperMotor::stop() { this->stop_motor = true; }

/**
 * Gets the action currently queued for the motor.
 */
Action StepperMotor::getQueuedAction() { return this->queued_action; }

/**
 * Adds the desired action as the next action to perform for this motor.
 *
 * This is a queue of length 1.
 */
void StepperMotor::queueAction(Action action) { this->queued_action = action; }

/**
 * Gets the current state of the motor.
 */
State StepperMotor::getState() { return this->state; }

/**
 * Sets the current working state of the motor.
 */
void StepperMotor::setState(State state) { this->state = state; }

/**
 * Gets the current half step delay of the motor.
 */
uint64_t StepperMotor::getHalfStepDelay() { return this->half_step_delay; }

/**
 * Starts the motor moving softly.
 *
 * Starts the speed slow and ramps it up.
 *
 * @param steps_remaining A pointer to the number of steps remaining for this
 * move. Will not go past this many steps, even if the number of steps is too
 * few to complete the soft start. The value will be decremented to keep steps
 * aligned with the caller. If NULL, then will be ignored.
 * @param initial_speed_half_step_delay The half step delay in micro seconds for
 * the initial step of the soft start. The speed will be increased from here.
 */
void StepperMotor::softStart(uint64_t* steps_remaining,
                             uint64_t initial_speed_half_step_delay) {
  // Get the bounding limit switch for the current direction.
  int ls = (this->getDir() == LEFT_DIR) ? LS_LEFT : LS_RIGHT;

  uint64_t full_speed_hs_delay = this->getHalfStepDelay();
  uint64_t current_speed_hs_delay = SM_SOFT_START_HALF_DELAY;

  if (steps_remaining == NULL) {
    while (current_speed_hs_delay > full_speed_hs_delay && !LS_TRIGGERED(ls)) {
      this->stepExact(current_speed_hs_delay);
      current_speed_hs_delay -= SM_SOFT_START_INCREASE_FACTOR;
    }
  } else {
    while (*steps_remaining > 0 &&
           current_speed_hs_delay > full_speed_hs_delay && !LS_TRIGGERED(ls)) {
      this->stepExact(current_speed_hs_delay);
      (*steps_remaining)--;
      current_speed_hs_delay -= SM_SOFT_START_INCREASE_FACTOR;
    }
  }
}

/**
 * Move `steps` number of steps in the provided direction.
 *
 * @param steps The number of steps to move.
 * @param dir The direction to move in.
 * @param soft_start Whether to soft start the movement.
 */
void StepperMotor::moveSteps(uint64_t steps, direction_t dir, bool soft_start) {
  uint64_t steps_remaining = steps;

  // Determine which limit switch will be the edge of this direction.
  int limit_switch = (dir == LEFT_DIR) ? LS_LEFT : LS_RIGHT;

  // Change the motor direction.
  this->setDir(dir);

  // Provide a soft start if requested.
  if (soft_start) this->softStart(&steps_remaining, SM_SOFT_START_HALF_DELAY);

  // Move the rest of the steps at the default speed.
  while (steps_remaining > 0 && !LS_TRIGGERED(limit_switch)) {
    this->step();
    steps_remaining--;
  }
};

/**
 * Moves the window to an absolute position in steps.
 *
 * @param step The absolute step position to move the motor to.
 */
void StepperMotor::moveToPosition(uint64_t step, bool soft_start) {
  uint64_t current_step_position = this->getPosition();

  // Determine the number of steps required to make up the difference between
  // the current step position and the desired step position, as well as the
  // direction needed to get there.
  uint64_t step_delta;
  direction_t dir;
  if (step > current_step_position) {
    dir = OPEN_DIR;
    step_delta = step - current_step_position;
  } else {
    dir = CLOSE_DIR;
    step_delta = current_step_position - step;
  }

  // Move the steps to move to the desired position.
  this->moveSteps(step_delta, dir, soft_start);
};

/**
 * Moves the window open to a certain percentage.
 *
 * @param percent The open percentage to set the window to.
 */
void StepperMotor::moveToPositionPercentage(float percent, bool soft_start) {
  // Clamp and convert the percentage to the equivalent position in steps.
  uint64_t step_position = this->percentageToSteps(CLAMP(percent, 0.0, 100.0));

  // Move to that position.
  this->moveToPosition(step_position, soft_start);
};

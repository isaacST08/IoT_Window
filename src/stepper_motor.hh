#ifndef STEPPER_MOTOR_HH
#define STEPPER_MOTOR_HH
#include <pico/types.h>
#include <stdbool.h>
#include <stdint.h>

#include "limit_switch.h"

#define SM_MS8_MIN_HALF_DELAY 85
#define SM_MS16_MIN_HALF_DELAY 42
#define SM_MS32_MIN_HALF_DELAY 21  // Done
#define SM_MS64_MIN_HALF_DELAY 11  // Done

#define SM_MS8_MIN_HALF_DELAY_QUIET 602
#define SM_MS16_MIN_HALF_DELAY_QUIET 301
#define SM_MS32_MIN_HALF_DELAY_QUIET 161
#define SM_MS64_MIN_HALF_DELAY_QUIET 75

namespace stepper_motor {

enum class Action { NONE, OPEN, CLOSE };

enum class State { OPEN, OPENING, CLOSED, CLOSING, STOPPED };

struct StepperMotorPins {
  uint enable;
  uint direction;
  uint pulse;
  uint ms1;
  uint ms2;
};

// struct StepperMotorLimitSwitches {
//   LimitSwitch* closed;
//   LimitSwitch* open;
//   LimitSwitch* home;
// };

// typedef struct StepperMotor {
//   enum StepperMotorAction queued_action;
//   enum StepperMotorState state;
//   struct StepperMotorPins pins;
//   // struct StepperMotorLimitSwitches limit_switches;
//   bool quiet_mode;
//   bool stop_motor;
//   int64_t step_position;
//   uint64_t half_step_delay;
//   float speed;
// } StepperMotor;

class StepperMotor {
 public:
  StepperMotor(uint enable_pin, uint direction_pin, uint pulse_pin,
               uint micro_step_1_pin, uint micro_step_2_pin,
               uint initial_micro_step, float initial_speed);

  uint getMicroStep();
  uint getMicroStepInt();
  void setMicroStep(uint micro_step);

  void enable();
  void disable();

  void setDir(bool dir);
  bool getDir();
  void swapDir();

  void setSpeed(float speed);
  float getSpeed();

  void setQuietMode(bool mode);

  uint64_t getPosition();
  int getPositionPercentage();

  void stepExact(uint64_t half_step_delay);
  void step();

  void home();
  bool close();
  bool open();

  void stop();

 private:
  Action queued_action;
  State state;
  struct StepperMotorPins pins;
  // struct StepperMotorLimitSwitches limit_switches;
  bool quiet_mode;
  bool stop_motor;
  int64_t step_position;
  uint64_t half_step_delay;
  float speed;
};

}  // namespace stepper_motor

// void smInit(StepperMotor* sm, uint enable_pin, uint direction_pin,
//             uint pulse_pin, uint micro_step_1_pin, uint micro_step_2_pin,
//             // LimitSwitch* closed_limit_switch, LimitSwitch*
//             open_limit_switch,
//             // LimitSwitch* home_limit_switch,
//             uint initial_micro_step, float initial_speed);
//
// uint smGetMicroStep(StepperMotor* sm);
// uint smGetMicroStepInt(StepperMotor* sm);
// void smSetMicroStep(StepperMotor* sm, uint micro_step);
//
// void smEnable(StepperMotor* sm);
// void smDisable(StepperMotor* sm);
//
// void smSetDir(StepperMotor* sm, bool dir);
// bool smGetDir(StepperMotor* sm);
// void smSwapDir(StepperMotor* sm);
//
// void smSetSpeed(StepperMotor* sm, float speed);
// float smGetSpeed(StepperMotor* sm);
//
// void smSetQuietMode(StepperMotor* sm, bool mode);
//
// uint64_t smGetPosition(StepperMotor* sm);
// int smGetPositionPercentage(StepperMotor* sm);
//
// void smStepExact(StepperMotor* sm, uint64_t half_step_delay);
// void smStep(StepperMotor* sm);
//
// void smHome(StepperMotor* sm);
// bool smClose(StepperMotor* sm);
// bool smOpen(StepperMotor* sm);
//
// void smStop(StepperMotor* sm);

// // ----- OLD -----
//
// void sm_enable();
// void sm_disable();
//
// void sm_set_speed_ptr(uint64_t* speed);
//
// void sm_step(uint64_t half_delay_us);
//
// void sm_set_dir(int dir);
//
// int get_micro_step();
//
// void sm_home();
// bool sm_close();
// bool sm_open();
// void sm_stop();

#endif

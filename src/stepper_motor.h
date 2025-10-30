#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H
#include <pico/types.h>
#include <stdbool.h>
#include <stdint.h>

enum StepperMotorAction { NONE, OPEN, CLOSE };

struct StepperMotorPins {
  uint enable;
  uint direction;
  uint pulse;
  uint ms1;
  uint ms2;
};

typedef struct StepperMotor {
  enum StepperMotorAction queued_action;
  struct StepperMotorPins pins;
  bool quiet_mode;
  bool stop_motor;
  int64_t step_position;
  uint64_t half_step_delay;
  uint8_t speed;
} StepperMotor;

void smInit(StepperMotor* sm, uint enable_pin, uint direction_pin,
            uint pulse_pin, uint micro_step_1_pin, uint micro_step_2_pin,
            uint initial_micro_step, uint8_t initial_speed);

uint smGetMicroStep(StepperMotor* sm);
uint smGetMicroStepInt(StepperMotor* sm);
void smSetMicroStep(StepperMotor* sm, uint micro_step);

void smEnable(StepperMotor* sm);
void smDisable(StepperMotor* sm);

void smSetDir(StepperMotor* sm, bool dir);
bool smGetDir(StepperMotor* sm);
void smSwapDir(StepperMotor* sm);

void smSetSpeed(StepperMotor* sm, uint8_t speed);
uint64_t smGetSpeed(StepperMotor* sm);

void smSetQuietMode(StepperMotor* sm, bool mode);

uint64_t smGetPosition(StepperMotor* sm);

void smStepExact(StepperMotor* sm, uint64_t half_step_delay);
void smStep(StepperMotor* sm);

void smHome(StepperMotor* sm);
bool smClose(StepperMotor* sm);
bool smOpen(StepperMotor* sm);

void smStop(StepperMotor* sm);

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

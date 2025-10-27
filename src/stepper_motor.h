#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H
#include <stdint.h>

// #define SM_FULL_STEPS_PER_REV 200
// #define SM_FULL_STEPS_PER_MM 100

// #define LEFT 1
// #define RIGHT 0

// #define SM_OPEN 0
// #define SM_CLOSE 100 * SM_FULL_STEPS_PER_MM + SM_OPEN  // 10cm

void sm_enable();
void sm_disable();
void sm_step(uint64_t half_delay_us);

void sm_set_dir(int dir);

int get_micro_step();

void sm_home();

#endif

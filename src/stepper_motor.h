#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H
#include <stdint.h>

#define SM_FULL_STEPS_PER_REV 200

void sm_enable();
void sm_disable();
void sm_step(uint64_t half_delay_us);

#endif

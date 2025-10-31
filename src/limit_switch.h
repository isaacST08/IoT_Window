#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include <pico/types.h>
#include <stdbool.h>

typedef struct LimitSwitch {
  uint trigger_pin;
  bool triggered_condition;  // The state to match when the switch is triggered.
} LimitSwitch;

void lsInit(LimitSwitch* ls, uint trigger_pin, bool triggered_condition,
            bool pull_up);
bool lsTriggered(LimitSwitch* ls);

#endif

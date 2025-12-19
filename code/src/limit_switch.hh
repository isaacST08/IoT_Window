#ifndef LIMIT_SWITCH_HH
#define LIMIT_SWITCH_HH

#include <pico/types.h>
#include <stdbool.h>

#define LS_TRIGGERED(ls) (gpio_get(ls) == 1)

#endif

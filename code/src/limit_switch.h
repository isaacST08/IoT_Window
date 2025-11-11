#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include <pico/types.h>
#include <stdbool.h>

#define LS_TRIGGERED(ls) (gpio_get(ls) == 1)

#endif

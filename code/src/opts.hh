#ifndef IOT_WINDOW_OPTS_HH
#define IOT_WINDOW_OPTS_HH
#include "common.hh"
#include "pins.hh"

#define DEFAULT_WINDOW_WIDTH_MM 200

#define INVERT_DIRECTION 0
#define INVERT_DISPLAY_DIRECTION 0

#define CLOSED_SIDE RIGHT_SIDE

// Which limit switch is on which side.
#define LS_LEFT LS_1
#define LS_RIGHT LS_2

// Which side to home to.
#define HOME_SIDE RIGHT_SIDE

/** Initial speed of the motor in mm/s on boot up. */
#define INITIAL_MOTOR_SPEED 5.0

#endif

#ifndef IOT_WINDOW_OPTS_HH
#define IOT_WINDOW_OPTS_HH
#include "common.hh"
#include "pins.hh"

// If your limit switches are correctly positioned, you can set this value to
// slightly above the distance between your limit switches and the window will
// auto calibrate when it first gets fully opened.
#define DEFAULT_WINDOW_WIDTH_MM 630

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

#ifndef IOT_WINDOW_OPTS_HH
#define IOT_WINDOW_OPTS_HH
#include "common.hh"
#include "pins.hh"

// #define OPEN_POSITION_MM 0
// #define CLOSED_POSITION_MM 100

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

// **==============================================**
// ||          <<<<< BASIC OPTIONS >>>>>           ||
// **==============================================**

// #define SM_FULL_STEPS_PER_REV 200
// #define SM_FULL_STEPS_PER_MM 100
//
// #define LEFT 1
// #define RIGHT 0
//
// #define SM_OPEN 0
// #define SM_CLOSE 100 * SM_FULL_STEPS_PER_MM + SM_OPEN  // 10cm

// **=================================================**
// ||          <<<<< ADVANCED OPTIONS >>>>>           ||
// **=================================================**

// #define SM_FULL_STEPS_PER_REV 200
// #define SM_FULL_STEPS_PER_MM 100

#endif

#ifndef IOT_WINDOW_ADV_OPTS_H
#define IOT_WINDOW_ADV_OPTS_H
#include "common.h"
#include "opts.h"

// **======================================**
// ||          <<<<< SIDES >>>>>           ||
// **======================================**

#define OPEN_SIDE CLOSED_SIDE ^ 1

// **==============================================**
// ||          <<<<< STEPPER MOTOR >>>>>           ||
// **==============================================**

#define SM_FULL_STEPS_PER_REV 200
#define SM_FULL_STEPS_PER_MM 100

#define SM_OPEN_POS_FULL_STEPS 0 * SM_FULL_STEPS_PER_MM
#define SM_CLOSED_POS_FULL_STEPS WINDOW_WIDTH_MM* SM_FULL_STEPS_PER_MM

#define LEFT_DIR LEFT_SIDE ^ INVERT_DIRECTION
#define RIGHT_DIR RIGHT_SIDE ^ INVERT_DIRECTION
#define HOME_DIR HOME_SIDE ^ INVERT_DIRECTION

// **===============================================**
// ||          <<<<< LIMIT SWITCHES >>>>>           ||
// **===============================================**

// #if (HOME_SIDE == RIGHT_SIDE)
// #define LS_HOME LS_RIGHT
// #else
// #define LS_HOME LS_LEFT
// #endif
#define LS_HOME (HOME_SIDE == RIGHT_SIDE) ? LS_RIGHT : LS_LEFT

#endif

#ifndef IOT_WINDOW_ADV_OPTS_HH
#define IOT_WINDOW_ADV_OPTS_HH
#include "common.hh"
#include "opts.hh"

// **======================================**
// ||          <<<<< SIDES >>>>>           ||
// **======================================**

#define OPEN_SIDE (CLOSED_SIDE ^ 1)

// **==============================================**
// ||          <<<<< STEPPER MOTOR >>>>>           ||
// **==============================================**

#define SM_FULL_STEPS_PER_REV 200
#define SM_FULL_STEPS_PER_MM 100

#define SM_QUIET_MODE_ADDITIONAL_US 200

#define SM_SMALLEST_MS 64
#define SM_LARGEST_MS 8

#define MIN_US_PER_HALF_SMALLEST_MS 20

#define LEFT_DIR (LEFT_SIDE ^ INVERT_DIRECTION)
#define RIGHT_DIR (RIGHT_SIDE ^ INVERT_DIRECTION)
#define HOME_DIR (HOME_SIDE ^ INVERT_DIRECTION)
#define CLOSE_DIR (CLOSED_SIDE ^ INVERT_DIRECTION)
#define OPEN_DIR (OPEN_SIDE ^ INVERT_DIRECTION)

#define CALIBRATION_SPEED_PRIMARY 5
#define CALIBRATION_SPEED_SECONDARY 1

// **===============================================**
// ||          <<<<< LIMIT SWITCHES >>>>>           ||
// **===============================================**

// #if (HOME_SIDE == RIGHT_SIDE)
// #define LS_HOME LS_RIGHT
// #else
// #define LS_HOME LS_LEFT
// #endif
#define LS_HOME ((HOME_SIDE == RIGHT_SIDE) ? LS_RIGHT : LS_LEFT)
#define LS_CLOSED ((CLOSED_SIDE == RIGHT_SIDE) ? LS_RIGHT : LS_LEFT)
#define LS_OPEN ((OPEN_SIDE == RIGHT_SIDE) ? LS_RIGHT : LS_LEFT)

// **=========================================**
// ||          <<<<< POSITION >>>>>           ||
// **=========================================**

#if (HOME_SIDE == CLOSED_SIDE)
#define WINDOW_CLOSED_STEP_POSITION 0
#define DEFAULT_WINDOW_OPEN_STEP_POSITION \
  (DEFAULT_WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS)
#else
#define WINDOW_CLOSED_STEP_POSITION                                  \
  (DEFAULT_WINDOW_WIDTH_MM * SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS * \
   (1 - CLOSE_DIR * 2))
#define WINDOW_OPEN_STEP_POSITION 0
#endif

// **===========================================**
// ||          <<<<< NETWORKING >>>>>           ||
// **===========================================**

/*
 * Wi-Fi will try to connect with a timeout. It will start with the minimum
 * timeout and double each time the timeout is reached until the maximum timeout
 * value is reached, at which point every following attempt will use the max
 * timeout value.
 *
 * This allows for the first few timeouts to be fast and allows for a faster
 * wifi connection, but still allows for longer connection timeouts if required.
 */
#define WIFI_CONNECTION_MIN_TIMOUT 1000
#define WIFI_CONNECTION_MAX_TIMEOUT 30000
#define WIFI_CONNECTION_MAX_ATTEMPTS -1  // Set to -1 for no max.

#endif

#ifndef STEPPER_MOTOR_HH
#define STEPPER_MOTOR_HH

#include <lwip/apps/mqtt.h>

#include <common.hh>
// #include <lwip/arch.h>
// #include <pico/types.h>
// #include <stdbool.h>
#include <stdint.h>

#include "action_queue.hh"
// #include "limit_switch.h"

typedef u8_t micro_step_t;

#define SM_MS8_MIN_HALF_DELAY 85
#define SM_MS16_MIN_HALF_DELAY 42
#define SM_MS32_MIN_HALF_DELAY 21  // Done
#define SM_MS64_MIN_HALF_DELAY 11  // Done

#define SM_MS8_MIN_HALF_DELAY_QUIET 602
#define SM_MS16_MIN_HALF_DELAY_QUIET 301
#define SM_MS32_MIN_HALF_DELAY_QUIET 161
#define SM_MS64_MIN_HALF_DELAY_QUIET 75

#define SM_MS_MIN_HALF_DELAY(ms)         \
  ((ms == 64)   ? SM_MS64_MIN_HALF_DELAY \
   : (ms == 32) ? SM_MS32_MIN_HALF_DELAY \
   : (ms == 16) ? SM_MS16_MIN_HALF_DELAY \
   : (ms == 8)  ? SM_MS8_MIN_HALF_DELAY  \
                : SM_MS8_MIN_HALF_DELAY)

#define SM_MS_MIN_HALF_DELAY_QUIET(ms)         \
  ((ms == 64)   ? SM_MS64_MIN_HALF_DELAY_QUIET \
   : (ms == 32) ? SM_MS32_MIN_HALF_DELAY_QUIET \
   : (ms == 16) ? SM_MS16_MIN_HALF_DELAY_QUIET \
   : (ms == 8)  ? SM_MS8_MIN_HALF_DELAY_QUIET  \
                : SM_MS8_MIN_HALF_DELAY_QUIET)

#define SM_SOFT_START_HALF_DELAY 1000
#define SM_SOFT_START_INCREASE_FACTOR 5

#define SM_ARG_BUFFER_SIZE 256

namespace stepper_motor {

// Action enum is defined in action_queue.hh

enum class State { OPEN, OPENING, CLOSED, CLOSING, STOPPED };

struct StepperMotorPins {
  uint enable;
  uint direction;
  uint pulse;
  uint ms1;
  uint ms2;
};

// struct StepperMotorLimitSwitches {
//   LimitSwitch* closed;
//   LimitSwitch* open;
//   LimitSwitch* home;
// };

// typedef struct StepperMotor {
//   enum StepperMotorAction queued_action;
//   enum StepperMotorState state;
//   struct StepperMotorPins pins;
//   // struct StepperMotorLimitSwitches limit_switches;
//   bool quiet_mode;
//   bool stop_motor;
//   int64_t step_position;
//   uint64_t half_step_delay;
//   float speed;
// } StepperMotor;

class StepperMotor {
 public:
  StepperMotor(uint enable_pin, uint direction_pin, uint pulse_pin,
               uint micro_step_1_pin, uint micro_step_2_pin,
               uint initial_micro_step, float initial_speed,
               mqtt_client_t* mqtt_client);

  // --- Parameters ---
  bool publish_updates;
  action::ActionQueue action_queue;

  // --- Basic ---
  void enable();
  void disable();

  // --- Direction ---
  direction_t getDir();
  void setDir(direction_t dir);
  void swapDir();

  // --- Quiet Mode ---
  bool getQuietMode();
  void setQuietMode(bool mode);

  // --- Micro Steps ---
  uint getMicroStep();
  uint getMicroStepInt();
  void setMicroStep(uint micro_step);

  // --- Speed ---
  float getSpeed();
  void setSpeed(float speed);

  uint64_t getHalfStepDelay();

  // --- Position ---
  uint64_t getPosition();
  int getPositionPercentage();
  float getPositionPercentageExact();

  float stepsToPercentage(uint64_t steps);
  uint64_t percentageToSteps(float percentage);

  // --- Steps ---
  void stepExact(uint64_t half_step_delay);
  void step();

  void moveSteps(uint64_t steps, direction_t dir, bool soft_start);

  // --- Movement ---
  void stop();

  void home();
  bool open();
  bool close();

  void moveToPosition(uint64_t step, bool soft_start);
  void moveToPositionPercentage(float percent, bool soft_start);

  void softStart(uint64_t* steps_remaining,
                 uint64_t initial_speed_half_step_delay);

  // --- Action Queueing ---
  bool hasQueuedActions();

  // --- States ---
  State getState();
  void setState(State state);
  void updateState();

  // --- MQTT ---
  bool basicMqttPublish(const char* topic, const char* payload, u8_t qos,
                        u8_t retain);

  void publishSpeed();
  void publishQuietMode();
  void publishPosition();
  void publishState();
  void publishMicroSteps();
  void publishHalfStepDelay();
  void publishAll();

 private:
  State state;
  struct StepperMotorPins pins;
  // struct StepperMotorLimitSwitches limit_switches;
  bool quiet_mode;
  bool stop_motor;
  int64_t step_position;
  uint64_t half_step_delay;
  float speed;
  float quiet_speed;
  mqtt_client_t* mqtt_client;
};

static void mqttPubRequestCb(void* arg, err_t result);

}  // namespace stepper_motor

#endif

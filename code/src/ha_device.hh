#ifndef HA_DEVICE_HH
#define HA_DEVICE_HH

#include <lwip/apps/mqtt.h>
#include <stdbool.h>

#include "ha_device_info.hh"
#include "mqtt_topics.hh"
#include "stepper_motor.hh"

// #define HA_DEVICE_MQTT_DISCOVERY_TOPIC_MAIN_WINDOW \
//   "homeassistant/cover/" HA_DEVICE_ID "/config"
// dev  |-> device
// ids  |-> identifiers
// name |-> name
// mf   |-> manufacturer
// mdl  |-> model
// sw   |-> sw_version
// hw   |-> hw_version
// sn   |-> serial_number
// o    |-> origin
// cmps |-> components
#define HA_DEVICE_MQTT_DISCOVERY_MSG                              \
  "{"                                                             \
  "\"dev\":{" /* device */                                        \
  "\"ids\":\"" HA_DEVICE_ID                                       \
  "\"," /* identifiers */                                         \
  "\"name\":\"" HA_DEVICE_ID                                      \
  "\"," /* name */                                                \
  "\"mf\":\"" HA_DEVICE_MANUFACTURER                              \
  "\"," /* manufacturer */                                        \
  "\"mdl\":\"" HA_DEVICE_MODEL                                    \
  "\"," /* model */                                               \
  "\"sw\":\"" HA_DEVICE_SOFTWARE_VERSION                          \
  "\"," /* sw_version */                                          \
  "\"sn\":\"" HA_DEVICE_SERIAL_NUMBER                             \
  "\"," /* serial_number */                                       \
  "\"hw\":\"" HA_DEVICE_HARDWARE_VERSION                          \
  "\"" /* hw_version */                                           \
  "},"                                                            \
  "\"o\":{" /* origin */                                          \
  "\"name\":\"foobar\","                                          \
  "\"sw\":\"" HA_DEVICE_SOFTWARE_VERSION                          \
  "\"" /* sw_version */                                           \
  "},"                                                            \
  "\"cmps\":{" /* components */                                   \
                                                                  \
  /* Main Window Component */                                     \
  "\"" HA_DEVICE_ID                                               \
  "-Main_Window\":{"                                              \
  "\"name\":\"Main Window\","                                     \
  "\"unique_id\":\"" HA_DEVICE_ID                                 \
  "-Main_Window\","                                               \
  "\"optimistic\":\"false\","                                     \
  "\"availability\":{"                                            \
  "\"payload_available\":\"online\","                             \
  "\"payload_not_available\":\"offline\","                        \
  "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
  "\""                                                            \
  "},"                                                            \
  "\"p\":\"cover\","                                              \
  "\"device_class\":\"window\","                                  \
  "\"payload_open\":\"OPEN\","                                    \
  "\"payload_close\":\"CLOSE\","                                  \
  "\"payload_stop\":\"STOP\","                                    \
  "\"state_topic\":\"" MQTT_TOPIC_STATE_GENERAL                   \
  "\","                                                           \
  "\"command_topic\":\"" MQTT_TOPIC_COMMAND_GENERAL               \
  "\","                                                           \
  "\"position_topic\":\"" MQTT_TOPIC_STATE_POSITION_PERCENT       \
  "\","                                                           \
  "\"set_position_topic\":\"" MQTT_TOPIC_COMMAND_POSITION_PERCENT \
  "\","                                                           \
  "\"value_template\":\"{{ value }}\""                            \
  "},"                                                            \
                                                                  \
  /* Main Window Speed Component */                               \
  "\"" HA_DEVICE_ID                                               \
  "-Main_Window_Speed\":{"                                        \
  "\"name\":\"Main Window Speed\","                               \
  "\"unique_id\":\"" HA_DEVICE_ID                                 \
  "-Main_Window_Speed\","                                         \
  "\"optimistic\":\"false\","                                     \
  "\"availability\":{"                                            \
  "\"payload_available\":\"online\","                             \
  "\"payload_not_available\":\"offline\","                        \
  "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
  "\""                                                            \
  "},"                                                            \
  "\"p\":\"number\","                                             \
  "\"min\":0,"                                                    \
  "\"max\":255,"                                                  \
  "\"mode\":\"box\","                                             \
  "\"step\":\"0.01\","                                            \
  "\"device_class\":\"speed\","                                   \
  "\"unit_of_measurement\":\"mm/s\","                             \
  "\"state_topic\":\"" MQTT_TOPIC_STATE_SPEED                     \
  "\","                                                           \
  "\"command_topic\":\"" MQTT_TOPIC_COMMAND_SPEED                 \
  "\","                                                           \
  "\"value_template\":\"{{ float(value) }}\","                    \
  "\"command_template\":\"{{ value }}\""                          \
  "},"                                                            \
                                                                  \
  /* Main Window Position Steps Component */                      \
  "\"" HA_DEVICE_ID                                               \
  "-Main_Window_Position_Steps\":{"                               \
  "\"name\":\"Main Window Position Steps\","                      \
  "\"unique_id\":\"" HA_DEVICE_ID                                 \
  "-Main_Window_Position_Steps\","                                \
  "\"optimistic\":\"false\","                                     \
  "\"availability\":{"                                            \
  "\"payload_available\":\"online\","                             \
  "\"payload_not_available\":\"offline\","                        \
  "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
  "\""                                                            \
  "},"                                                            \
  "\"p\":\"number\","                                             \
  "\"min\":-10000000000000000000," /* 1e19 */                     \
  "\"max\":10000000000000000000,"  /* 1e19 */                     \
  "\"mode\":\"box\","                                             \
  "\"state_topic\":\"" MQTT_TOPIC_STATE_POSITION_STEPS            \
  "\","                                                           \
  "\"command_topic\":\"" MQTT_TOPIC_COMMAND_POSITION_STEPS        \
  "\""                                                            \
  "},"                                                            \
                                                                  \
  /* Main Window Position Millimeters Component */                \
  "\"" HA_DEVICE_ID                                               \
  "-Main_Window_Position_Millimeters\": {"                        \
  "\"name\":\"Main Window Position Millimeters\","                \
  "\"unique_id\":\"" HA_DEVICE_ID                                 \
  "-Main_Window_Position_Millimeters\","                          \
  "\"optimistic\":\"false\","                                     \
  "\"availability\":{"                                            \
  "\"payload_available\":\"online\","                             \
  "\"payload_not_available\":\"offline\","                        \
  "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
  "\""                                                            \
  "},"                                                            \
  "\"p\":\"number\","                                             \
  "\"min\":-100000000000," /* 1e */                               \
  "\"max\":1000000000000," /* 1e */                               \
  "\"mode\":\"box\","                                             \
  "\"step\":\"0.1\","                                             \
  "\"unit_of_measurement\":\"mm\","                               \
  "\"state_topic\":\"" MQTT_TOPIC_STATE_POSITION_MM               \
  "\","                                                           \
  "\"command_topic\":\"" MQTT_TOPIC_COMMAND_POSITION_MM           \
  "\""                                                            \
  "},"                                                            \
                                                                  \
  /* Quiet Mode Switch Component */                               \
  "\"" HA_DEVICE_ID                                               \
  "-Quiet_Mode_Switch\":{"                                        \
  "\"name\":\"Quiet Mode Switch\","                               \
  "\"unique_id\":\"" HA_DEVICE_ID                                 \
  "-Quiet_Mode_Switch\","                                         \
  "\"optimistic\":\"false\","                                     \
  "\"availability\":{"                                            \
  "\"payload_available\":\"online\","                             \
  "\"payload_not_available\":\"offline\","                        \
  "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
  "\""                                                            \
  "},"                                                            \
  "\"p\":\"switch\","                                             \
  "\"state_topic\":\"" MQTT_TOPIC_STATE_QUIET                     \
  "\","                                                           \
  "\"command_topic\":\"" MQTT_TOPIC_COMMAND_QUIET                 \
  "\""                                                            \
  "},"                                                            \
                                                                  \
  /* Micro Step Sensor */                                         \
  "\"" HA_DEVICE_ID                                               \
  "-Micro_Step_Sensor\":{"                                        \
  "\"name\":\"Micro Step Sensor\","                               \
  "\"unique_id\":\"" HA_DEVICE_ID                                 \
  "-Micro_Step_Sensor\","                                         \
  "\"optimistic\":\"false\","                                     \
  "\"availability\":{"                                            \
  "\"payload_available\":\"online\","                             \
  "\"payload_not_available\":\"offline\","                        \
  "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
  "\""                                                            \
  "},"                                                            \
  "\"p\":\"sensor\","                                             \
  "\"device_class\":null,"                                        \
  "\"state_topic\":\"" MQTT_TOPIC_SENSOR_MICRO_STEPS              \
  "\","                                                           \
  "\"qos\":1"                                                     \
  "},"                                                            \
                                                                  \
  /* Step Delay Sensor */                                         \
  "\"" HA_DEVICE_ID                                               \
  "-Half_Step_Delay_Sensor\":{"                                   \
  "\"name\":\"Half Step Delay\","                                 \
  "\"unique_id\":\"" HA_DEVICE_ID                                 \
  "-Half_Step_Delay_Sensor\","                                    \
  "\"optimistic\":\"false\","                                     \
  "\"availability\":{"                                            \
  "\"payload_available\":\"online\","                             \
  "\"payload_not_available\":\"offline\","                        \
  "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
  "\""                                                            \
  "},"                                                            \
  "\"p\":\"sensor\","                                             \
  "\"device_class\":\"duration\","                                \
  "\"unit_of_measurement\":\"µs\","                               \
  "\"state_topic\":\"" MQTT_TOPIC_SENSOR_HALF_STEP_DELAY          \
  "\""                                                            \
  "}"                                                             \
                                                                  \
  "},"                                                            \
  "\"qos\":0"                                                     \
  "}"

// #define HA_DEVICE_COMP_MAIN_WINDOW                                \
//   "" /* Main Window Component */                                  \
//   "\"name\":\"Main Window\","                                     \
//   "\"unique_id\":\"" HA_DEVICE_ID                                 \
//   "-Main_Window\","                                               \
//   "\"optimistic\":\"false\","                                     \
//   "\"availability\":{"                                            \
//   "\"payload_available\":\"online\","                             \
//   "\"payload_not_available\":\"offline\","                        \
//   "\"topic\":\"" MQTT_TOPIC_AVAILABILITY                          \
//   "\""                                                            \
//   "},"                                                            \
//   "\"device_class\":\"window\","                                  \
//   "\"payload_open\":\"OPEN\","                                    \
//   "\"payload_close\":\"CLOSE\","                                  \
//   "\"payload_stop\":\"STOP\","                                    \
//   "\"state_topic\":\"" MQTT_TOPIC_STATE_GENERAL                   \
//   "\","                                                           \
//   "\"command_topic\":\"" MQTT_TOPIC_COMMAND_GENERAL               \
//   "\","                                                           \
//   "\"position_topic\":\"" MQTT_TOPIC_STATE_POSITION_PERCENT       \
//   "\","                                                           \
//   "\"set_position_topic\":\"" MQTT_TOPIC_COMMAND_POSITION_PERCENT \
//   "\","                                                           \
//   "\"value_template\":\"{{ value }}\""
//
// #define HA_DEVICE_MQTT_DISCOVERY_MSG(component) \
//   "{"                                           \
//   "\"dev\": {" /* device */                     \
//   "\"ids\": \"" HA_DEVICE_ID                    \
//   "\"," /* identifiers */                       \
//   "\"name\": \"" HA_DEVICE_ID                   \
//   "\"," /* name */                              \
//   "\"mf\": \"" HA_DEVICE_MANUFACTURER           \
//   "\"," /* manufacturer */                      \
//   "\"mdl\": \"" HA_DEVICE_MODEL                 \
//   "\"," /* model */                             \
//   "\"sw\": \"" HA_DEVICE_SOFTWARE_VERSION       \
//   "\"," /* sw_version */                        \
//   "\"sn\": \"" HA_DEVICE_SERIAL_NUMBER          \
//   "\"," /* serial_number */                     \
//   "\"hw\": \"" HA_DEVICE_HARDWARE_VERSION       \
//   "\"" /* hw_version */                         \
//   "},"                                          \
//   "\"o\": {" /* origin */                       \
//   "\"name\": \"foobar\","                       \
//   "\"sw\": \"" HA_DEVICE_SOFTWARE_VERSION       \
//   "\"" /* sw_version */                         \
//   "}," component                                \
//   "\"qos\": 0"                                  \
//   "}"

bool basicMqttPublish(const char* topic, const char* payload, u8_t qos,
                      u8_t retain);
// void updateState(enum StepperMotorAction device_state);
void updateState(stepper_motor::Action device_state);
bool mqttDoConnect(mqtt_client_t* client);
void haDeviceSetup(mqtt_client_t* client, stepper_motor::StepperMotor* sm);
// void haDeviceSetup(mqtt_client_t* client, StepperMotor* stepper_motor);

// void publishStepperMotorSpeed();
// void publishStepperMotorQuietMode();
// void publishStepperMotorState();
// void publishStepperMotorPositionPercentage();
// void publishStepperMotorPositionSteps();
// void publishStepperMotorPositionMM();
// void publishAll();

#endif

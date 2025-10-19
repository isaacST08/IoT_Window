#ifndef HA_DEVICE_H
#define HA_DEVICE_H

// ----- Device Info -----
#define HA_DEVICE_ID "window_1"

#define HA_DEVICE_MANUFACTURER "Isaac Shiells Thomas"
#define HA_DEVICE_MODEL "Prototype"
#define HA_DEVICE_SOFTWARE_VERSION "3.0"
#define HA_DEVICE_HARDWARE_VERSION "1.0"
#define HA_DEVICE_SERIAL_NUMBER "1"

// ----- MQTT Topics -----
#define MQTT_TOPIC_BASE "hass/" HA_DEVICE_ID "/"
#define MQTT_TOPIC_AVAILABILITY MQTT_TOPIC_BASE "avail"
#define MQTT_TOPIC_STATE_GENERAL MQTT_TOPIC_BASE "state/gnrl"
#define MQTT_TOPIC_STATE_SPEED MQTT_TOPIC_BASE "state/speed"
#define MQTT_TOPIC_STATE_QUIET MQTT_TOPIC_BASE "state/quiet"
#define MQTT_TOPIC_STATE_POSITION_STEPS MQTT_TOPIC_BASE "state/steps"
#define MQTT_TOPIC_STATE_POSITION_MM MQTT_TOPIC_BASE "state/mm"
#define MQTT_TOPIC_STATE_POSITION_PERCENT MQTT_TOPIC_BASE "state/percent"
#define MQTT_TOPIC_COMMAND_GENERAL MQTT_TOPIC_BASE "cmd/general"
#define MQTT_TOPIC_COMMAND_SPEED MQTT_TOPIC_BASE "cmd/speed"
#define MQTT_TOPIC_COMMAND_QUIET MQTT_TOPIC_BASE "cmd/quiet"
#define MQTT_TOPIC_COMMAND_POSITION_STEPS MQTT_TOPIC_BASE "cmd/steps"
#define MQTT_TOPIC_COMMAND_POSITION_MM MQTT_TOPIC_BASE "cmd/mm"
#define MQTT_TOPIC_COMMAND_POSITION_PERCENT MQTT_TOPIC_BASE "cmd/percent"

// ----- Device Discovery -----
#define HA_DEVICE_MQTT_DISCOVERY_TOPIC "hass/device/" HA_DEVICE_ID "/config"
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
  "\"min\":1,"                                                    \
  "\"max\":20,"                                                   \
  "\"mode\":\"box\","                                             \
  "\"state_topic\":\"" MQTT_TOPIC_STATE_SPEED                     \
  "\","                                                           \
  "\"command_topic\":\"" MQTT_TOPIC_COMMAND_SPEED                 \
  "\""                                                            \
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
  "\"min\":0,"                                                    \
  "\"max\":2147483648," /* 2^31 */                                \
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
  "\"min\":0,"                                                    \
  "\"max\":2147483648," /* 2^31 */                                \
  "\"mode\":\"box\","                                             \
  "\"step\":\"0.001\","                                           \
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

#endif

#ifndef HA_DEVICE_H
#define HA_DEVICE_H

#define HA_DEVICE_ID "window_1"

#define HA_DEVICE_MANUFACTURER "Isaac Shiells Thomas"
#define HA_DEVICE_MODEL "Prototype"
#define HA_DEVICE_SOFTWARE_VERSION "1.0"
#define HA_DEVICE_HARDWARE_VERSION "1.0"
#define HA_DEVICE_SERIAL_NUMBER "1"

#define HA_DEVICE_MQTT_DISCOVERY_TOPIC                                         \
  "homeassistant/device/" HA_DEVICE_ID "/config"
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
#define HA_DEVICE_MQTT_DISCOVERY_MSG                                           \
  "{"                                                                          \
  "  \"dev\": {"                                                               \
  "    \"ids\": \"" HA_DEVICE_ID "\","                                         \
  "    \"name\": \"" HA_DEVICE_ID "\","                                        \
  "    \"mf\": \"" HA_DEVICE_MANUFACTURER "\","                                \
  "    \"mdl\": \"" HA_DEVICE_MODEL "\","                                      \
  "    \"sw\": \"" HA_DEVICE_SOFTWARE_VERSION "\","                            \
  "    \"sn\": \"" HA_DEVICE_SERIAL_NUMBER "\","                               \
  "    \"hw\": \"" HA_DEVICE_HARDWARE_VERSION "\""                             \
  "  },"                                                                       \
  "  \"o\": {"                                                                 \
  "    \"name\": \"foobar\","                                                  \
  "    \"sw\": \"" HA_DEVICE_SOFTWARE_VERSION "\""                             \
  "  },"                                                                       \
  "  \"cmps\": {},"                                                            \
  "  \"qos\": 0"                                                               \
  "}"

#define MQTT_TOPIC_BASE "homeassistant/" HA_DEVICE_ID "/"
#define MQTT_TOPIC_AVAILABILITY MQTT_TOPIC_BASE "availability"
#define MQTT_TOPIC_STATE_GENERAL MQTT_TOPIC_BASE "state/general"
#define MQTT_TOPIC_STATE_SPEED MQTT_TOPIC_BASE "state/speed"
#define MQTT_TOPIC_STATE_QUIET MQTT_TOPIC_BASE "state/quiet"
#define MQTT_TOPIC_STATE_POSITION_STEPS MQTT_TOPIC_BASE "state/steps"
#define MQTT_TOPIC_STATE_POSITION_MM MQTT_TOPIC_BASE "state/mm"
#define MQTT_TOPIC_STATE_POSITION_PERCENT MQTT_TOPIC_BASE "state/percent"
#define MQTT_TOPIC_COMMAND_GENERAL MQTT_TOPIC_BASE "command/general"
#define MQTT_TOPIC_COMMAND_SPEED MQTT_TOPIC_BASE "command/speed"
#define MQTT_TOPIC_COMMAND_QUIET MQTT_TOPIC_BASE "command/quiet"
#define MQTT_TOPIC_COMMAND_POSITION_STEPS MQTT_TOPIC_BASE "command/steps"
#define MQTT_TOPIC_COMMAND_POSITION_MM MQTT_TOPIC_BASE "command/mm"
#define MQTT_TOPIC_COMMAND_POSITION_PERCENT MQTT_TOPIC_BASE "command/percent"

#endif

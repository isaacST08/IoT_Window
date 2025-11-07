#include "ha_device.hh"

#include <cyw43_configport.h>
#include <hardware/gpio.h>
#include <lwip/apps/mqtt.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "advanced_opts.h"
#include "common.h"
#include "opts.h"
#include "pico/cyw43_arch.h"
#include "pins.h"
#include "secrets.h"
#include "stepper_motor.hh"

#define MQTT_SUBSCRIBE(client, topic, err)                                \
  for (int i = 0; i < 3; i++) {                                           \
    cyw43_arch_lwip_begin();                                              \
    err = mqtt_sub_unsub(client, topic, 1, mqttSubRequestCb, NULL, true); \
    cyw43_arch_lwip_end();                                                \
    if (err != ERR_OK) {                                                  \
      printf("Failed to subscribe to %s with error %d\n", topic, err);    \
      for (int i = 0; i < 2; i++) {                                       \
        gpio_put(YELLOW_LED_PIN, 1);                                      \
        sleep_ms(100);                                                    \
        gpio_put(YELLOW_LED_PIN, 0);                                      \
        sleep_ms(100);                                                    \
      }                                                                   \
    } else                                                                \
      break;                                                              \
  }                                                                       \
  sleep_ms(150);

// #define WINDOW_OPEN_FULL_STEPS 10000

enum InPub {
  OTHER,
  GENERAL,
  POSITION_PERCENT,
  POSITION_STEPS,
  POSITION_MM,
  QUIET,
  SPEED
};

// **===============================================**
// ||          <<<<< STATIC VARIABLES>>>>>          ||
// **===============================================**

// Stores the ID of the incoming message topic.
// Used to transfer topic from the incoming topic callback to the incoming data
// callback.
static enum InPub inpub_id;

static mqtt_client_t* mqtt_client;

static stepper_motor::StepperMotor* window_sm;

// **===============================================**
// ||          <<<<< LED ERROR CODES >>>>>          ||
// **===============================================**

// Blue | On;
// Red  | 3x | 150ms:100ms
void pubErrLedCode() {
  gpio_put(BLUE_LED_PIN, 1);
  for (int i = 0; i < 3; i++) {
    gpio_put(RED_LED_PIN, 1);
    sleep_ms(150);
    gpio_put(RED_LED_PIN, 0);
    sleep_ms(100);
  }
  gpio_put(BLUE_LED_PIN, 0);
}

// Blue   | On;
// Yellow | 3x | 150ms:100ms
void subErrLedCode() {
  gpio_put(BLUE_LED_PIN, 1);
  for (int i = 0; i < 3; i++) {
    gpio_put(YELLOW_LED_PIN, 1);
    sleep_ms(150);
    gpio_put(YELLOW_LED_PIN, 0);
    sleep_ms(100);
  }
  gpio_put(BLUE_LED_PIN, 0);
}

// Blue | 5x | 100ms:50ms
void connectionErrLedCode() {
  for (int i = 0; i < 3; i++) {
    gpio_put(BLUE_LED_PIN, 1);
    sleep_ms(100);
    gpio_put(BLUE_LED_PIN, 0);
    sleep_ms(50);
  }
}

// Blue + Yellow | 3x | 150ms:100ms
void pubDiscoveryMsgErrLedCode() {
  for (int i = 0; i < 3; i++) {
    gpio_put(BLUE_LED_PIN, 1);
    gpio_put(YELLOW_LED_PIN, 1);
    sleep_ms(150);
    gpio_put(BLUE_LED_PIN, 0);
    gpio_put(YELLOW_LED_PIN, 0);
    sleep_ms(100);
  }
}
// **===============================================**
// ||          <<<<< BASIC FUNCTIONS >>>>>          ||
// **===============================================**

static void mqttPubRequestCb(void* arg, err_t result);

bool basicMqttPublish(const char* topic, const char* payload, u8_t qos,
                      u8_t retain) {
  // const char* pub_payload = payload;

  err_t err;
  cyw43_arch_lwip_begin();
  err = mqtt_publish(mqtt_client, topic, payload, strlen(payload), qos, retain,
                     mqttPubRequestCb, NULL);
  cyw43_arch_lwip_end();
  if (err != ERR_OK) {
    // setup_successful = false;
    printf("Publish err: %d\n", err);
    // pubDiscoveryMsgErrLedCode();
    return false;
  }

  return true;
}

// **===========================================================**
// ||          <<<<< HA DEVICE PUBLISH FUNCTIONS >>>>>          ||
// **===========================================================**

void publishStepperMotorSpeed() {
  char buf[16];
  sprintf(buf, "%.2f", window_sm->getSpeed());
  basicMqttPublish(MQTT_TOPIC_STATE_SPEED, buf, 1, 0);
}

void publishStepperMotorQuietMode() {
  basicMqttPublish(MQTT_TOPIC_STATE_QUIET,
                   (window_sm->getQuietMode()) ? "ON" : "OFF", 1, 0);
}

void publishStepperMotorPositionPercentage() {
  char buf[64];
#if INVERT_DISPLAY_DIRECTION
  sprintf(buf, "%d", -1 * window_sm->getPositionPercentage());
#else
  sprintf(buf, "%d", smGetPositionPercentage(window_stepper_motor));
#endif
  basicMqttPublish(MQTT_TOPIC_STATE_POSITION_PERCENT, buf, 1, 0);
}

void publishStepperMotorPositionSteps() {
  char buf[64];
#if INVERT_DISPLAY_DIRECTION
  sprintf(buf, "%lld", -1 * window_sm->getPosition());
#else
  sprintf(buf, "%lld", window_sm->getPosition());
#endif
  basicMqttPublish(MQTT_TOPIC_STATE_POSITION_STEPS, buf, 1, 0);
}

void publishStepperMotorPositionMM() {
  char buf[64];
  sprintf(buf, "%0.1f",
          (double)window_sm->getPosition() /
              (SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS));
#if INVERT_DISPLAY_DIRECTION
  sprintf(buf, "%0.1f",
          -1 * ((double)window_sm->getPosition() /
                (SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS)));
#else
  sprintf(buf, "%0.1f",
          (double)window_sm->getPosition() /
              (SM_FULL_STEPS_PER_MM * SM_SMALLEST_MS));
#endif
  basicMqttPublish(MQTT_TOPIC_STATE_POSITION_MM, buf, 1, 0);
}

void publishStepperMotorState() {
  char* payload;
  // switch (window_stepper_motor->state) {
  switch (window_sm->getState()) {
    case stepper_motor::State::OPEN:
      payload = "open";
      break;
    case stepper_motor::State::OPENING:
      payload = "opening";
      break;
    case stepper_motor::State::CLOSED:
      payload = "closed";
      break;
    case stepper_motor::State::CLOSING:
      payload = "closing";
      break;
    case stepper_motor::State::STOPPED:
      payload = "stopped";
      break;
  }
  basicMqttPublish(MQTT_TOPIC_STATE_GENERAL, payload, 1, 0);
}

void publishStepperMotorMicroSteps() {
  char buf[4];
  sprintf(buf, "%d", window_sm->getMicroStepInt());
  basicMqttPublish(MQTT_TOPIC_SENSOR_MICRO_STEPS, buf, 1, 0);
}

void publishStepperMotorHalfStepDelay() {
  char buf[16];
  sprintf(buf, "%llu", window_sm->getHalfStepDelay());
  basicMqttPublish(MQTT_TOPIC_SENSOR_HALF_STEP_DELAY, buf, 1, 0);
}

void publishAll() {
  publishStepperMotorSpeed();
  publishStepperMotorQuietMode();
  publishStepperMotorState();
  publishStepperMotorPositionPercentage();
  publishStepperMotorPositionSteps();
  publishStepperMotorPositionMM();
  publishStepperMotorMicroSteps();
  publishStepperMotorHalfStepDelay();
}

// **===============================================**
// ||          <<<<< MQTT CALL-BACKS >>>>>          ||
// **===============================================**

// ----- Function Implementations -----

/* Called when publish is complete either with success or failure */
static void mqttPubRequestCb(void* arg, err_t result) {
  if (result != ERR_OK) {
    printf("Publish result: %d\n", result);
    pubErrLedCode();
  }
}

static void mqttSubRequestCb(void* arg, err_t result) {
  for (int i = 0; i < 4; i++) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(150);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(75);
  }
  if (result != ERR_OK) {
    printf("Publish result: %d\n", result);
    subErrLedCode();
  }
}

static void mqttIncomingPublishCb(void* arg, const char* topic, u32_t tot_len) {
  printf("Incoming publish at topic %s with total length %u\n", topic,
         (unsigned int)tot_len);

  /* Decode topic string into a user defined reference */
  if (strcmp(topic, MQTT_TOPIC_COMMAND_GENERAL) == 0) {
    inpub_id = GENERAL;
  } else if (strcmp(topic, MQTT_TOPIC_COMMAND_POSITION_PERCENT) == 0) {
    inpub_id = POSITION_PERCENT;
  } else if (strcmp(topic, MQTT_TOPIC_COMMAND_POSITION_STEPS) == 0) {
    inpub_id = POSITION_STEPS;
  } else if (strcmp(topic, MQTT_TOPIC_COMMAND_POSITION_MM) == 0) {
    inpub_id = POSITION_MM;
  } else if (strcmp(topic, MQTT_TOPIC_COMMAND_QUIET) == 0) {
    inpub_id = QUIET;
  } else if (strcmp(topic, MQTT_TOPIC_COMMAND_SPEED) == 0) {
    inpub_id = SPEED;
  } else {
    /* For all other topics */
    inpub_id = OTHER;
  }
}

static void mqttIncomingDataCb(void* arg, const u8_t* data, u16_t len,
                               u8_t flags) {
  printf("Incoming publish payload with length %d, flags %u\n", len,
         (unsigned int)flags);

  if (flags & MQTT_DATA_FLAG_LAST) {
    /* Last fragment of payload received (or whole part if payload fits receive
       buffer See MQTT_VAR_HEADER_BUFFER_LEN)  */

    /* Call function or do action depending on reference, in this case inpub_id
     */
    switch (inpub_id) {
      case GENERAL: {
        if (len >= 4 && memcmp((char*)data, "OPEN", 4) == 0) {
          // basicMqttPublish(MQTT_TOPIC_STATE_GENERAL, "opening", 1, 0);

          window_sm->queueAction(stepper_motor::Action::OPEN);
          window_sm->setState(stepper_motor::State::OPENING);
          publishAll();
          // updateState(OPENING);
          // *queued_motor_move = OPEN;
          printf("Opening...\n");
          // bool open_success = sm_open();
          // basicMqttPublish(MQTT_TOPIC_STATE_GENERAL,
          //                  (open_success) ? "open" : "stopped", 1, 0);
        } else if (len >= 5 && memcmp((char*)data, "CLOSE", 5) == 0) {
          // basicMqttPublish(MQTT_TOPIC_STATE_GENERAL, "closing", 1, 0);

          window_sm->queueAction(stepper_motor::Action::CLOSE);
          window_sm->setState(stepper_motor::State::CLOSING);
          publishAll();

          // updateState(CLOSING);
          // *queued_motor_move = CLOSED;
          printf("Closing...\n");
          // bool close_success = sm_close();
          // basicMqttPublish(MQTT_TOPIC_STATE_GENERAL,
          //                  (close_success) ? "closed" : "stopped", 1, 0);
        } else if (len >= 4 && memcmp((char*)data, "STOP", 4) == 0) {
          window_sm->stop();
          // smStop(window_stepper_motor);
          // sm_stop();
          // basicMqttPublish(MQTT_TOPIC_STATE_GENERAL, "stopped", 1, 0);
          // updateState(STOPPED);
          publishAll();
        } else {
          printf("Unknown general command\n");
        }

        // printf("Do general command stuff\n");
        break;
      }
      case POSITION_PERCENT: {
        // /* Don't trust the publisher, check zero termination */
        // if (data[len - 1] == 0) {
        //   printf("mqtt_incoming_data_cb: %s\n", (const char*)data);
        // }
        printf("Do position percentage command stuff\n");

        //   int new_position_percent = atoi((char*)data);
        //   while (new_position_percent > 100)
        //     new_position_percent = new_position_percent / 10;
        //
        //   int new_position_steps =
        //       ((SM_CLOSED_POS_FULL_STEPS - SM_OPEN_POS_FULL_STEPS) *
        //        new_position_percent) /
        //       100;
        //   if (new_position_steps < 0)
        //     new_position_steps = new_position_steps * -1;
        //
        //   int position_change_steps = motor_position_steps -
        //   new_position_steps; printf(
        //       "Change position requested.\n\tSteps | New position: %d,
        //       Current " "position: %d, position change: %d\n",
        //       new_position_steps, motor_position_steps,
        //       position_change_steps);
        //
        //   int micro_step;
        //   switch (get_micro_step()) {
        //     case MS_8:
        //       micro_step = 8;
        //       break;
        //     case MS_16:
        //       micro_step = 16;
        //       break;
        //     case MS_32:
        //       micro_step = 32;
        //       break;
        //     case MS_64:
        //       micro_step = 64;
        //       break;
        //     default:
        //       micro_step = 8;
        //       break;
        //   }
        //   int steps = position_change_steps * micro_step;
        //   // (position_change * WINDOW_OPEN_FULL_STEPS * micro_step) / 100;
        //   if (steps < 0) {
        //     steps = steps * -1;
        //     sm_set_dir(LEFT_DIR);
        //   } else {
        //     sm_set_dir(RIGHT_DIR);
        //   }
        //   sm_enable();
        //   for (int i = 0; i < steps; i++) sm_step(100);
        //
        //   motor_position_steps = new_position_steps;
        //
        //   {
        //     int motor_pos_percent =
        //         motor_position_steps /
        //         (SM_CLOSED_POS_FULL_STEPS - SM_OPEN_POS_FULL_STEPS);
        //     char pub_payload_str[4];
        //     sprintf(pub_payload_str, "%d", motor_pos_percent);
        //     const char* pub_payload = (const char*)&pub_payload_str;
        //
        //     // u16_t pub_payload_len = 1 + (motor_pos_percent >= 10) +
        //     // (motor_pos_percent >= 100);
        //
        //     err_t err;
        //     u8_t qos = 1;
        //     u8_t retain = 1;
        //     err = mqtt_publish(mqtt_client,
        //     MQTT_TOPIC_STATE_POSITION_PERCENT,
        //                        pub_payload, strlen(pub_payload), qos, retain,
        //                        mqttPubRequestCb, NULL);
        //     // if (err != ERR_OK) {
        //     //   setup_successful = false;
        //     //   printf("Publish err: %d\n", err);
        //     //   pubDiscoveryMsgErrLedCode();
        //     // }
        //   }
        //
        break;
      }
      case POSITION_STEPS: {
        /* Don't trust the publisher, check zero termination */
        if (data[len - 1] == 0) {
          printf("mqtt_incoming_data_cb: %s\n", (const char*)data);
        }
        printf("Do position steps command stuff\n");
        break;
      }
      case POSITION_MM: {
        /* Don't trust the publisher, check zero termination */
        if (data[len - 1] == 0) {
          printf("mqtt_incoming_data_cb: %s\n", (const char*)data);
        }
        printf("Do position mm command stuff\n");
        break;
      }
      case QUIET: {
        /* Don't trust the publisher, check zero termination */
        // if (data[len - 1] == 0) {
        //   printf("mqtt_incoming_data_cb: %s\n", (const char*)data);
        // }
        printf("Do quiet command stuff\n");
        if (len >= 2 && memcmp((char*)data, "ON", 2) == 0)
          window_sm->setQuietMode(true);
        else if (len >= 3 && memcmp((char*)data, "OFF", 3) == 0)
          window_sm->setQuietMode(false);

        publishStepperMotorQuietMode();
        publishStepperMotorSpeed();
        publishStepperMotorMicroSteps();

        break;
      }
      case SPEED: {
        float new_speed = atof((char*)data);
        if (new_speed < 0.01) new_speed = 0.01;
        printf("Setting motor speed to %f\n", new_speed);
        window_sm->setSpeed(new_speed);
        publishStepperMotorSpeed();
        publishStepperMotorMicroSteps();
        break;
      }
      case OTHER: {
        printf("mqtt_incoming_data_cb: Ignoring payload...\n");
        break;
      }
    }
    // if (inpub_id == 0) {
    //   /* Don't trust the publisher, check zero termination */
    //   if (data[len - 1] == 0) {
    //     printf("mqtt_incoming_data_cb: %s\n", (const char*)data);
    //   }
    // } else if (inpub_id == 1) {
    //   /* Call an 'A' function... */
    // } else {
    //   printf("mqtt_incoming_data_cb: Ignoring payload...\n");
    // }
  } else {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
    printf("TODO: Handel fragmented MQTT payloads.\n");
  }
}

static void mqttConnectionCb(mqtt_client_t* client, void* arg,
                             mqtt_connection_status_t status) {
  err_t err;
  if (status == MQTT_CONNECT_ACCEPTED) {
    // Setup callback for incoming publish requests.
    mqtt_set_inpub_callback(client, mqttIncomingPublishCb, mqttIncomingDataCb,
                            arg);

    // Subscribe to the general command topic.
    // err = mqtt_subscribe(client, MQTT_TOPIC_COMMAND_SPEED, 1,
    // mqttSubRequestCb,
    //                      arg);
    // err = mqtt_subscribe(client, MQTT_TOPIC_COMMAND_GENERAL, 1,
    //                      mqttSubRequestCb, arg);
    // err = mqtt_subscribe(client, MQTT_TOPIC_COMMAND_POSITION_PERCENT, 1,
    //                      mqttSubRequestCb, arg);
    // err = mqtt_subscribe(client, MQTT_TOPIC_COMMAND_POSITION_STEPS, 1,
    //                      mqttSubRequestCb, arg);
    // err = mqtt_subscribe(client, MQTT_TOPIC_COMMAND_POSITION_MM, 1,
    //                      mqttSubRequestCb, arg);
    // err = mqtt_subscribe(client, MQTT_TOPIC_COMMAND_QUIET, 1,
    // mqttSubRequestCb,
    //                      arg);
    MQTT_SUBSCRIBE(client, MQTT_TOPIC_COMMAND_GENERAL, err);
    MQTT_SUBSCRIBE(client, MQTT_TOPIC_COMMAND_SPEED, err)
    MQTT_SUBSCRIBE(client, MQTT_TOPIC_COMMAND_QUIET, err)
    MQTT_SUBSCRIBE(client, MQTT_TOPIC_COMMAND_POSITION_MM, err)
    MQTT_SUBSCRIBE(client, MQTT_TOPIC_COMMAND_POSITION_STEPS, err)
    MQTT_SUBSCRIBE(client, MQTT_TOPIC_COMMAND_POSITION_PERCENT, err)
    // cyw43_arch_lwip_begin();
    // err = mqtt_sub_unsub(client, MQTT_TOPIC_COMMAND_GENERAL, 1,
    //                      mqttSubRequestCb, NULL, true);
    // if (err != ERR_OK) {
    //   printf("cmd gnrl mqtt_sub return %d\n", err);
    //   for (int i = 0; i < 6; i++) {
    //     gpio_put(YELLOW_LED_PIN, 1);
    //     sleep_ms(100);
    //     gpio_put(YELLOW_LED_PIN, 0);
    //     sleep_ms(100);
    //   }
    // }
    // err = mqtt_sub_unsub(client, MQTT_TOPIC_COMMAND_SPEED, 1,
    // mqttSubRequestCb,
    //                      NULL, true);
    // while (err != ERR_OK) {
    //   for (int i = 0; i < 6; i++) {
    //     gpio_put(RED_LED_PIN, 1);
    //     gpio_put(GREEN_LED_PIN, 1);
    //     sleep_ms(100);
    //     gpio_put(RED_LED_PIN, 0);
    //     gpio_put(GREEN_LED_PIN, 0);
    //     sleep_ms(100);
    //   }
    //   cyw43_arch_lwip_begin();
    //   err = mqtt_sub_unsub(client, MQTT_TOPIC_COMMAND_SPEED, 1,
    //                        mqttSubRequestCb, NULL, true);
    //   cyw43_arch_lwip_end();
    // }

    // if (err != ERR_OK) {
    //   printf("cmd speed mqtt_sub return %d\n", err);
    //   for (int i = 0; i < 6; i++) {
    //     gpio_put(YELLOW_LED_PIN, 1);
    //     sleep_ms(100);
    //     gpio_put(YELLOW_LED_PIN, 0);
    //     sleep_ms(100);
    //   }
    // }
    // cyw43_arch_lwip_end();
  } else {
    for (int i = 0; i < 2; i++) {
      gpio_put(RED_LED_PIN, 1);
      gpio_put(YELLOW_LED_PIN, 1);
      sleep_ms(200);
      gpio_put(RED_LED_PIN, 0);
      gpio_put(YELLOW_LED_PIN, 0);
      sleep_ms(100);
    }
    mqttDoConnect(client);
  }
}

// **==============================================**
// ||          <<<<< MQTT FUNCTIONS >>>>>          ||
// **==============================================**

bool mqttDoConnect(mqtt_client_t* client) {
  struct mqtt_connect_client_info_t ci;
  err_t err;

  mqtt_client = client;

  // Setup empty client info struct.
  memset(&ci, 0, sizeof(ci));

  // Set client info.
  ci.client_id = HA_DEVICE_ID;
  ci.client_user = MQTT_USER;
  ci.client_pass = MQTT_PASSWORD;
  ci.keep_alive = 60;

  // Define the
  ip4_addr_t mqtt_broker_ipaddr;
  ipaddr_aton(MQTT_SERVER, &mqtt_broker_ipaddr);

  // Initiate client and connect to server.
  cyw43_arch_lwip_begin();
  err = mqtt_client_connect(client, &mqtt_broker_ipaddr, MQTT_PORT,
                            mqttConnectionCb, 0, &ci);
  cyw43_arch_lwip_end();

  // Flash blue LEDs if an error occurs.
  if (err != ERR_OK) {
    switch (err) {
      case ERR_RTE:
        printf("MQTT ERROR: Routing problem.\n");
        break;
      default:
        printf("mqtt_connect return %d\n", err);
        break;
    }
    printf("Wifi connection status: %d\n",
           cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA));
    connectionErrLedCode();
    sleep_ms(200);
  }

  return (err == ERR_OK);
}

// **===================================================**
// ||          <<<<< HA DEVICE FUNCTIONS >>>>>          ||
// **===================================================**

/**
 * Sets-up the home assistant device by sending the MQTT discovery message and
 * publishing that the device is available.
 *
 * \param client The MQTT client to use to publish the messages.
 * \param sm The stepper motor for the window.
 */
void haDeviceSetup(mqtt_client_t* client, stepper_motor::StepperMotor* sm) {
  // bool setup_successful = true;

  char buf[64];

  window_sm = sm;

  err_t err;

  // HA Device Discovery Message.
  do {
    cyw43_arch_lwip_begin();
    {
      const char* pub_payload = HA_DEVICE_MQTT_DISCOVERY_MSG;
      u8_t qos = 2;    /* 0 1 or 2, see MQTT specification */
      u8_t retain = 1; /* Retain discovery message for if/when HA restarts. */
      err = mqtt_publish(client, HA_DEVICE_MQTT_DISCOVERY_TOPIC, pub_payload,
                         strlen(pub_payload), qos, retain, mqttPubRequestCb,
                         NULL);
      if (err != ERR_OK) {
        // setup_successful = false;
        printf("Publish err: %d\n", err);
        pubDiscoveryMsgErrLedCode();
      }
    }
    cyw43_arch_lwip_end();
    if (err != ERR_OK) sleep_ms(300);
  } while (err != ERR_OK);

  // Make device available in home assistant.
  do {
    cyw43_arch_lwip_begin();
    {
      const char* pub_payload = "online";
      u8_t qos = 1;     /* 0 1 or 2, see MQTT specification */
      u8_t retain = 0;  // Don't retain the online state of this device.
      err = mqtt_publish(client, MQTT_TOPIC_AVAILABILITY, pub_payload,
                         strlen(pub_payload), qos, retain, mqttPubRequestCb,
                         NULL);
      if (err != ERR_OK) {
        // setup_successful = false;
        printf("Publish err: %d\n", err);
        pubErrLedCode();
      }
    }
    cyw43_arch_lwip_end();
    if (err != ERR_OK) sleep_ms(300);
  } while (err != ERR_OK);

  // Acquire lwIP locks.
  // cyw43_arch_lwip_begin();

  // // HA Device Discovery Message.
  // {
  //   const char* pub_payload = HA_DEVICE_MQTT_DISCOVERY_MSG;
  //   err_t err;
  //   u8_t qos = 2;    /* 0 1 or 2, see MQTT specification */
  //   u8_t retain = 1; /* Retain discovery message for if/when HA restarts. */
  //   err =
  //       mqtt_publish(client, HA_DEVICE_MQTT_DISCOVERY_TOPIC, pub_payload,
  //                    strlen(pub_payload), qos, retain, mqttPubRequestCb,
  //                    NULL);
  //   if (err != ERR_OK) {
  //     setup_successful = false;
  //     printf("Publish err: %d\n", err);
  //     pubDiscoveryMsgErrLedCode();
  //   }
  // }

  // Make device available in home assistant.
  // {
  //   const char* pub_payload = "online";
  //   err_t err;
  //   u8_t qos = 1;     /* 0 1 or 2, see MQTT specification */
  //   u8_t retain = 0;  // Don't retain the online state of this device.
  //   err =
  //       mqtt_publish(client, MQTT_TOPIC_AVAILABILITY, pub_payload,
  //                    strlen(pub_payload), qos, retain, mqttPubRequestCb,
  //                    NULL);
  //   if (err != ERR_OK) {
  //     setup_successful = false;
  //     printf("Publish err: %d\n", err);
  //     pubErrLedCode();
  //   }
  // }

  // Publish device states.
  cyw43_arch_lwip_begin();
  publishAll();
  cyw43_arch_lwip_end();

  // return setup_successful;
}

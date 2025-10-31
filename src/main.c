#include <cyw43_country.h>
#include <cyw43_ll.h>
#include <hardware/gpio.h>
#include <lwip/apps/mqtt_opts.h>
#include <lwip/err.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip_addr.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "ha_device.h"
#include "limit_switch.h"
#include "lwip/apps/mqtt.h"
#include "network.h"
#include "pico/async_context.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pins.h"
#include "secrets.h"
#include "stepper_motor.h"

int main() {
  stdio_init_all();
  sleep_ms(1000);

  printf("Program Start\n");

  printf("Initializing Pins... ");
  init_pins();

  printf("done.\n");

  // Wave LEDs.
  for (int i = 0; i < 3; i++) {
    int ms = 100;
    gpio_put(RED_LED_PIN, 1);
    sleep_ms(ms);
    gpio_put(GREEN_LED_PIN, 1);
    sleep_ms(ms);
    gpio_put(YELLOW_LED_PIN, 1);
    sleep_ms(ms);
    gpio_put(BLUE_LED_PIN, 1);
    sleep_ms(ms);
    gpio_put(RED_LED_PIN, 0);
    sleep_ms(ms);
    gpio_put(GREEN_LED_PIN, 0);
    sleep_ms(ms);
    gpio_put(YELLOW_LED_PIN, 0);
    sleep_ms(ms);
    gpio_put(BLUE_LED_PIN, 0);
    sleep_ms(ms);
  }

  // Initialize the network connection.
  printf("Initializing Networking...");
  if (network_init() != 0) {
    // printf("Failed to initialize network connection.\n");
    printf("Failed! Exiting.\n");
    return -1;
  } else {
    printf("Success.\n");
    // printf("Network setup successful.\n");
  }

  printf("MEMSIZE size: %d\n", MEM_SIZE);
  printf("RINGBUF size: %d\n", MQTT_OUTPUT_RINGBUF_SIZE);

  // enum DeviceState *queued_motor_move;
  // *queued_motor_move = STOPPED;
  // enum DeviceState queued_motor_move = STOPPED;
  // uint64_t motor_speed = 40;

  // sm_set_speed_ptr(&motor_speed);

  // for (int i = 0; i < 15; i++) {
  //   gpio_put(RED_LED_PIN, 1);
  //   gpio_put(GREEN_LED_PIN, 1);
  //   sleep_ms(300);
  //   gpio_put(RED_LED_PIN, 0);
  //   gpio_put(GREEN_LED_PIN, 0);
  //   sleep_ms(300);
  // }

  // Initialize Stepper Motor.
  StepperMotor stepper_motor;
  smInit(&stepper_motor, SM_ENABLE_PIN, SM_DIR_PIN, SM_PULSE_PIN, SM_MS1_PIN,
         SM_MS2_PIN, MS_64, 1);

  // MQTT Setup
  printf("Setting up mqtt...\n");
  cyw43_arch_lwip_begin();
  mqtt_client_t *mqtt_client = mqtt_client_new();
  cyw43_arch_lwip_end();
  printf("Insurring mqtt connection...\n");
  bool mqtt_setup_success;
  do {
    mqtt_setup_success = mqttDoConnect(mqtt_client);
  } while (!mqtt_setup_success);
  printf("MQTT setup finished\n");

  // printf("Setting up mqtt...\n");
  // cyw43_arch_lwip_begin();
  // mqtt_client_t *mqtt_client = mqtt_client_new();
  // if (mqtt_client != NULL) {
  //   mqtt_do_connect(mqtt_client);
  // } else {
  //   for (int i = 0; i < 15; i++) {
  //     gpio_put(RED_LED_PIN, 1);
  //     gpio_put(GREEN_LED_PIN, 1);
  //     sleep_ms(300);
  //     gpio_put(RED_LED_PIN, 0);
  //     gpio_put(GREEN_LED_PIN, 0);
  //     sleep_ms(300);
  //   }
  // }
  // printf("MQTT setup finished\n");
  // cyw43_arch_lwip_end();

  haDeviceSetup(mqtt_client, &stepper_motor);
  // bool ha_dev_discovery_msg_sent;
  // do {
  //   ha_dev_discovery_msg_sent = haDeviceSetup(mqtt_client, &stepper_motor);
  // } while (!ha_dev_discovery_msg_sent);

  // mqtt_publish_device_config(mqtt_client, NULL);
  // example_publish(mqtt_client, NULL);
  // cyw43_arch_lwip_end();

  // Turn on the board led.
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  cyw43_arch_lwip_begin();
  smEnable(&stepper_motor);
  printf("Homeing...\n");
  smHome(&stepper_motor);
  printf("Homeing Complete.\n");
  cyw43_arch_lwip_end();

  while (true) {
    gpio_put(RED_LED_PIN, cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) !=
                              CYW43_LINK_JOIN);

    if (stepper_motor.queued_action != SM_ACTION_NONE) {
      printf("Queued motor move: %d\n", stepper_motor.queued_action);
      switch (stepper_motor.queued_action) {
        case SM_ACTION_OPEN: {
          printf("Open motor activated\n");
          bool open_success = smOpen(&stepper_motor);
          // if (open_success) {
          //   // basicMqttPublish(MQTT_TOPIC_STATE_GENERAL, "open", 1, 0);
          //   // updateState(OPEN);
          // } else {
          //   printf("Stopping motor\n");
          //   updateState(STOPPED);
          // }
          // queued_motor_move = STOPPED;
          break;
        }
        case SM_ACTION_CLOSE: {
          bool close_success = smClose(&stepper_motor);
          // if (close_success) {
          //   // basicMqttPublish(MQTT_TOPIC_STATE_GENERAL, "close", 1, 0);
          //   updateState(CLOSED);
          // } else {
          //   updateState(STOPPED);
          // }
          // queued_motor_move = STOPPED;
          break;
        }
        case SM_ACTION_NONE:
          break;
          // default:
          //   // queued_motor_move = STOPPED;
          //   break;
      }
      // queued_motor_move = STOPPED;
      publishAll();
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(250);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(250);
  }
}

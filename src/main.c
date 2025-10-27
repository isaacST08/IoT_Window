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

#include "ha_device.h"
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
  init_pins(MS_16);
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
  if (network_init() != 0) {
    printf("Failed to initialize network connection.\n");
    return -1;
  } else {
    printf("Network setup successful.\n");
  }

  printf("MEMSIZE size: %d\n", MEM_SIZE);
  printf("RINGBUF size: %d\n", MQTT_OUTPUT_RINGBUF_SIZE);

  // for (int i = 0; i < 10; i++) {
  //   for (int i = 0; i < 2; i++) {
  //     gpio_put(YELLOW_LED_PIN, 1);
  //     sleep_ms(100);
  //     gpio_put(YELLOW_LED_PIN, 0);
  //     sleep_ms(50);
  //   }
  //   sleep_ms(700);
  // }
  // sleep_ms(10 * 1000);

  // MQTT Setup
  printf("Setting up mqtt...\n");
  cyw43_arch_lwip_begin();
  mqtt_client_t *mqtt_client = mqtt_client_new();
  cyw43_arch_lwip_end();
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

  bool ha_dev_discovery_msg_sent;
  do {
    ha_dev_discovery_msg_sent = haDeviceSetup(mqtt_client);
  } while (!ha_dev_discovery_msg_sent);

  // mqtt_publish_device_config(mqtt_client, NULL);
  // example_publish(mqtt_client, NULL);
  // cyw43_arch_lwip_end();

  // Turn on the board led.
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  sm_enable();
  // for (int i = 0; i < 10000; i++) {
  //   sm_step(200);
  // }
  printf("Homeing...\n");
  sm_home();

  while (true) {
    gpio_put(RED_LED_PIN, cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) !=
                              CYW43_LINK_JOIN);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(250);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(250);
  }
}

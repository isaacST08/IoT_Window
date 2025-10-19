#include <cyw43_country.h>
#include <cyw43_ll.h>
#include <hardware/gpio.h>
#include <lwip/apps/mqtt_opts.h>
#include <lwip/err.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip_addr.h>
#include <pico/time.h>
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

void mqtt_do_connect(mqtt_client_t *client);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
                               mqtt_connection_status_t status);
static void mqtt_sub_request_cb(void *arg, err_t result);
static void mqtt_incoming_publish_cb(void *arg, const char *topic,
                                     u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
                                  u8_t flags);
static void mqtt_pub_request_cb(void *arg, err_t result);

void mqtt_do_connect(mqtt_client_t *client) {
  struct mqtt_connect_client_info_t ci;
  err_t err;

  // Setup empty client info struct.
  memset(&ci, 0, sizeof(ci));

  // Set client info.
  ci.client_id = HA_DEVICE_ID;
  ci.client_user = MQTT_USER;
  ci.client_pass = MQTT_PASSWORD;
  ci.keep_alive = 60;

  ip4_addr_t mqtt_broker_ipaddr;
  ipaddr_aton(MQTT_SERVER, &mqtt_broker_ipaddr);
  // struct ip4_addr mqtt_ip;
  // mqtt_ip.addr = MQTT_SERVER;
  // printf("MQTT HAVE TLS: %d\n", MQTT_HAVE_TLS);

  // Initiate client and connect to server.
  gpio_put(BLUE_LED_PIN, 1);
  err = mqtt_client_connect(client, &mqtt_broker_ipaddr, MQTT_PORT,
                            mqtt_connection_cb, 0, &ci);
  while (err != ERR_OK) {
    gpio_put(YELLOW_LED_PIN, gpio_get(YELLOW_LED_PIN) ^ 1);
    sleep_ms(500);
    err = mqtt_client_connect(client, &mqtt_broker_ipaddr, MQTT_PORT,
                              mqtt_connection_cb, 0, &ci);
  };
  gpio_put(BLUE_LED_PIN, 0);
  gpio_put(YELLOW_LED_PIN, 0);

  // Flash red LED if an error occurs.
  if (err != ERR_OK) {
    printf("mqtt_connect return %d\n", err);
    for (int i = 0; i < 6; i++) {
      gpio_put(RED_LED_PIN, 1);
      gpio_put(BLUE_LED_PIN, 1);
      sleep_ms(200);
      gpio_put(RED_LED_PIN, 0);
      gpio_put(BLUE_LED_PIN, 0);
      sleep_ms(200);
    }
  }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
                               mqtt_connection_status_t status) {
  err_t err;
  if (status == MQTT_CONNECT_ACCEPTED) {
    // Setup callback for incoming publish requests.
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb,
                            mqtt_incoming_data_cb, arg);

    // Subscribe to the general command topic.
    err = mqtt_subscribe(client, MQTT_TOPIC_COMMAND_GENERAL, 0,
                         mqtt_sub_request_cb, arg);

    if (err != ERR_OK) {
      for (int i = 0; i < 6; i++) {
        gpio_put(YELLOW_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(YELLOW_LED_PIN, 0);
        sleep_ms(100);
      }
    }
  } else {
    for (int i = 0; i < 2; i++) {
      gpio_put(RED_LED_PIN, 1);
      gpio_put(YELLOW_LED_PIN, 1);
      sleep_ms(200);
      gpio_put(RED_LED_PIN, 0);
      gpio_put(YELLOW_LED_PIN, 0);
      sleep_ms(100);
    }
    mqtt_do_connect(client);
  }
}

static void mqtt_sub_request_cb(void *arg, err_t result) {
  for (int i = 0; i < 4; i++) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(150);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(75);
  }
}

static int inpub_id;
static void mqtt_incoming_publish_cb(void *arg, const char *topic,
                                     u32_t tot_len) {
  printf("Incoming publish at topic %s with total length %u\n", topic,
         (unsigned int)tot_len);

  /* Decode topic string into a user defined reference */
  if (strcmp(topic, MQTT_TOPIC_COMMAND_GENERAL) == 0) {
    inpub_id = 0;
  } else if (topic[0] == 'A') {
    /* All topics starting with 'A' might be handled in the same way */
    inpub_id = 1;
  } else {
    /* For all other topics */
    inpub_id = 2;
  }
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
                                  u8_t flags) {
  printf("Incoming publish payload with length %d, flags %u\n", len,
         (unsigned int)flags);

  if (flags & MQTT_DATA_FLAG_LAST) {
    /* Last fragment of payload received (or whole part if payload fits receive
       buffer See MQTT_VAR_HEADER_BUFFER_LEN)  */

    /* Call function or do action depending on reference, in this case inpub_id
     */
    if (inpub_id == 0) {
      /* Don't trust the publisher, check zero termination */
      if (data[len - 1] == 0) {
        printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
      }
    } else if (inpub_id == 1) {
      /* Call an 'A' function... */
    } else {
      printf("mqtt_incoming_data_cb: Ignoring payload...\n");
    }
  } else {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
  }
}

void example_publish(mqtt_client_t *client, void *arg) {
  const char *pub_payload = "PubSubHubLubJub";
  err_t err;
  u8_t qos = 2;    /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such a crappy payload... */
  err =
      mqtt_publish(client, MQTT_TOPIC_STATE_GENERAL, pub_payload,
                   strlen(pub_payload), qos, retain, mqtt_pub_request_cb, arg);
  if (err != ERR_OK) {
    printf("Publish err: %d\n", err);
    for (int i = 0; i < 3; i++) {
      gpio_put(BLUE_LED_PIN, 1);
      gpio_put(YELLOW_LED_PIN, 1);
      sleep_ms(150);
      gpio_put(BLUE_LED_PIN, 0);
      gpio_put(YELLOW_LED_PIN, 0);
      sleep_ms(100);
    }
  }
}

void mqtt_publish_device_config(mqtt_client_t *client, void *arg) {
  const char *pub_payload = HA_DEVICE_MQTT_DISCOVERY_MSG;
  err_t err;
  u8_t qos = 2;    /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such a crappy payload... */
  err =
      mqtt_publish(client, HA_DEVICE_MQTT_DISCOVERY_TOPIC, pub_payload,
                   strlen(pub_payload), qos, retain, mqtt_pub_request_cb, arg);
  while (err != ERR_OK) {
    printf("Publish err: %d\n", err);
    for (int i = 0; i < 3; i++) {
      gpio_put(BLUE_LED_PIN, 1);
      gpio_put(YELLOW_LED_PIN, 1);
      sleep_ms(150);
      gpio_put(BLUE_LED_PIN, 0);
      gpio_put(YELLOW_LED_PIN, 0);
      sleep_ms(100);
    }
    err = mqtt_publish(client, HA_DEVICE_MQTT_DISCOVERY_TOPIC, pub_payload,
                       strlen(pub_payload), qos, retain, mqtt_pub_request_cb,
                       arg);
  }
}

/* Called when publish is complete either with sucess or failure */
static void mqtt_pub_request_cb(void *arg, err_t result) {
  if (result != ERR_OK) {
    printf("Publish result: %d\n", result);
    for (int i = 0; i < 3; i++) {
      gpio_put(BLUE_LED_PIN, 1);
      sleep_ms(150);
      gpio_put(BLUE_LED_PIN, 0);
      sleep_ms(100);
    }
  }
}

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

  for (int i = 0; i < 10; i++) {
    for (int i = 0; i < 2; i++) {
      gpio_put(YELLOW_LED_PIN, 1);
      sleep_ms(100);
      gpio_put(YELLOW_LED_PIN, 0);
      sleep_ms(50);
    }
    sleep_ms(700);
  }
  // sleep_ms(10 * 1000);

  // MQTT Setup
  printf("Setting up mqtt...\n");
  cyw43_arch_lwip_begin();
  mqtt_client_t *mqtt_client = mqtt_client_new();
  if (mqtt_client != NULL) {
    mqtt_do_connect(mqtt_client);
  } else {
    for (int i = 0; i < 15; i++) {
      gpio_put(RED_LED_PIN, 1);
      gpio_put(GREEN_LED_PIN, 1);
      sleep_ms(300);
      gpio_put(RED_LED_PIN, 0);
      gpio_put(GREEN_LED_PIN, 0);
      sleep_ms(300);
    }
  }
  printf("MQTT setup finished\n");

  mqtt_publish_device_config(mqtt_client, NULL);
  example_publish(mqtt_client, NULL);
  cyw43_arch_lwip_end();

  // Turn on the board led.
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  sm_enable();
  for (int i = 0; i < 10000; i++) {
    sm_step(500);
  }

  while (true) {
    gpio_put(RED_LED_PIN, cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) !=
                              CYW43_LINK_JOIN);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(250);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(250);
  }
}

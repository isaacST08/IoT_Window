#include "network.h"

#include <cyw43.h>
#include <cyw43_country.h>
#include <cyw43_ll.h>
#include <hardware/gpio.h>
#include <lwip/netif.h>

#include "lwip/apps/mqtt.h"
#include "pico/async_context.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pins.h"
#include "secrets.h"

/**
 * Initializes the network connection for the pico.
 *
 * \returns -1 on failure, 0 on success.
 */
int network_init() {
  // Initialize Wi-Fi chip.
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_CANADA)) {
    printf("Wi-Fi init failed\n");
    return -1;
  }

  // Blink board LED 5 times fast and leave on to signal chip initialization
  // success.
  for (int i = 0; i < 10; i++) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(50);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);
  }

  // Enable station mode to be able to act as a client device.
  cyw43_arch_enable_sta_mode();

  // Set the device host name.
  cyw43_arch_lwip_begin();
  struct netif *n = &cyw43_state.netif[CYW43_ITF_STA];
  netif_set_hostname(n, "IoT_Window");
  netif_set_up(n);
  // netif_is_up(n);
  cyw43_arch_lwip_end();

  // Begin async connection to the Wi-Fi network.
  cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD,
                                CYW43_AUTH_WPA2_AES_PSK  // Auth
  );

  sleep_ms(700);
  // Loop until Wi-Fi connects.
  // Blink the red LED twice every second whilst not connected.
  int wifi_state = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
  // while (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) !=
  //        CYW43_LINK_JOIN) {
  switch (wifi_state) {
    case CYW43_LINK_NONET:
      printf("WiFi: Could not find network.\n");
      break;
    default:
      printf("Wifi state: %d\n", wifi_state);
      break;
  }
  while (wifi_state != CYW43_LINK_JOIN) {
    switch (wifi_state) {
      case CYW43_LINK_NONET:
        printf("WiFi: Could not find network.\n");
        break;
      default:
        printf("Wifi state: %d\n", wifi_state);
        break;
    }
    for (int i = 0; i < 2; i++) {
      gpio_put(RED_LED_PIN, 1);
      sleep_ms(100);
      gpio_put(RED_LED_PIN, 0);
      sleep_ms(50);
    }
    sleep_ms(700);
    wifi_state = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
  }

  // Blink the board led fast 10 times to indicate a Wi-Fi connection was
  // made.
  for (int i = 0; i < 10; i++) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(50);
  }

  // Return 0 for success.
  return 0;
}

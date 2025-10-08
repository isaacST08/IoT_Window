#include "pico/async_context.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "secrets.h"
#include <cyw43_country.h>
#include <cyw43_ll.h>
#include <hardware/gpio.h>

#define RED_LED_PIN 0

int main() {
  gpio_init(RED_LED_PIN);
  gpio_set_dir(RED_LED_PIN, GPIO_OUT);

  stdio_init_all();
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_CANADA)) {
    printf("Wi-Fi init failed\n");
    return -1;
  }

  for (int i = 0; i < 10; i++) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(50);
  }

  cyw43_arch_enable_sta_mode();
  cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD,
                                CYW43_AUTH_WPA2_AES_PSK // auth
  );

  // Loop until wifi connects.
  while (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) !=
         CYW43_LINK_JOIN) {

    for (int i = 0; i < 2; i++) {
      gpio_put(RED_LED_PIN, 1);
      sleep_ms(100);
      gpio_put(RED_LED_PIN, 0);
      sleep_ms(50);
    }
    sleep_ms(700);
  }
  for (int i = 0; i < 10; i++) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(50);
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

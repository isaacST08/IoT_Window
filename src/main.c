#include "network.h"
#include "pico/async_context.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pins.h"
#include "secrets.h"
#include <cyw43_country.h>
#include <cyw43_ll.h>
#include <hardware/gpio.h>

int main() {
  gpio_init(RED_LED_PIN);
  gpio_set_dir(RED_LED_PIN, GPIO_OUT);

  stdio_init_all();

  // Initialize the network connection.
  if (network_init() != 0) {
    printf("Failed to initialize network connection.\n");
    return -1;
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

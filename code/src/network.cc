#include "network.hh"

// #include <cyw43.h>
// #include <cyw43_country.h>
// #include <cyw43_ll.h>
// #include <hardware/gpio.h>
// #include <lwip/netif.h>
#include <hardware/watchdog.h>
#include <math.h>
#include <pico/cyw43_arch.h>
// #include <pico/error.h>
// #include <pico/platform/compiler.h>

#include "advanced_opts.hh"
// #include "pins.hh"
#include "secrets.hh"

void setHostname() {
  // Acquire the network lock.
  cyw43_arch_lwip_begin();

  // Set the host name.
  struct netif* n = &cyw43_state.netif[CYW43_ITF_STA];
  netif_set_hostname(n, "IoT_Window");
  netif_set_up(n);

  // Release the network lock.
  cyw43_arch_lwip_end();
}

void wifiDisconnect() {
  cyw43_arch_disable_sta_mode();
  cyw43_arch_enable_sta_mode();
  setHostname();
}

/**
 * Attempts to connect to the predefined Wi-Fi network.
 *
 * @returns 0 on success, -1 if the Wi-Fi password is incorrect or the maximum
 * number of attempts was reached.
 */
int wifiConnect() {
  // Turn on the blue LED to signal that the Wi-Fi connection process is in
  // progress.
  gpio_put(BLUE_LED_PIN, 1);

  // Start the connection timeout at the predefined minimum.
  // This value will be doubled each time the connection fails due to timeout
  // until the maximum timeout value is reached.
  int connection_timeout = WIFI_CONNECTION_MIN_TIMOUT;

  // Record the status code of the connection attempt.
  int status_code;

  // Record the link state after each connection attempt.
  int link_state = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);

  int connect_attempt = 0;
  while (connect_attempt++ != WIFI_CONNECTION_MAX_ATTEMPTS &&
         link_state != CYW43_LINK_JOIN) {
    // Feed the watchdog during potentially long connection attempts.
    watchdog_update();

    // If not the first connection attempt, print out the attempt number and the
    // link state.
    if (connect_attempt > 1) {
      char* link_state_str;
      int link_state_str_len;
      switch (link_state) {
        case CYW43_LINK_DOWN:
          link_state_str = (char*)"down";
          link_state_str_len = 4;
          break;
        case CYW43_LINK_JOIN:
          link_state_str = (char*)"join";
          link_state_str_len = 4;
          break;
        case CYW43_LINK_FAIL:
          link_state_str = (char*)"fail";
          link_state_str_len = 4;
          break;
        case CYW43_LINK_NONET:
          link_state_str = (char*)"nonet";
          link_state_str_len = 5;
          break;
        case CYW43_LINK_BADAUTH:
          link_state_str = (char*)"badauth";
          link_state_str_len = 7;
          break;
        default:
          link_state_str = (char*)"";
          link_state_str_len = 0;
          break;
      }
      printf(
          "\033[0K"
          "(attempt: %d, link state: %s) "
          "\033[%dD",
          connect_attempt, link_state_str,
          27 + ((int)floor(log10(connect_attempt))) + link_state_str_len);
    }

    // Attempt to connect to the Wi-Fi network.
    status_code = cyw43_arch_wifi_connect_timeout_ms(
        WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, connection_timeout);

    switch (status_code) {
      // If the Wi-Fi connection failed with an incorrect password, immediately
      // return a failure without trying again.
      case PICO_ERROR_BADAUTH: {
        // Trying to connect to a network that we are already connected to will
        // also result in a bad auth, but technically it is still a success.
        if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) !=
            CYW43_LINK_JOIN) {
          gpio_put(BLUE_LED_PIN, 0);
          printf(
              "\033[0K"
              "WiFi: Incorrect password.\n");
          return -1;
        }
      }

      // When the Wi-Fi connection is successful, return a success.
      case 0: {
        gpio_put(BLUE_LED_PIN, 0);
        printf("\033[0K");  // Clear the stuff we printed.
        return 0;
      }

      // If the Wi-Fi connection failed due to a timeout, increase the timeout
      // up to the maximum and continue with the rest of the loop.
      case PICO_ERROR_TIMEOUT: {
        connection_timeout =
            MIN(connection_timeout * 2, WIFI_CONNECTION_MAX_TIMEOUT);
      }

      // Perform a cycle of the loop if any other error occurred.
      default: {
        // Cycle the board LED to signal that the process has not frozen.
        for (int i = 0; i < 3; i++) {
          cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
          sleep_ms(50);
          cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
          sleep_ms(100);
          watchdog_update();  // Feed watchdog during retry
        }

        // Update the link state.
        link_state = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
      }
    }
  }

  printf("\033[0K");          // Clear the stuff we printed.
  gpio_put(BLUE_LED_PIN, 0);  // Turn the blue LED back off.
  return (link_state == CYW43_LINK_JOIN) ? 0 : -1;
}

/**
 * Initializes the network connection for the pico.
 *
 * \returns -1 on failure, 0 on success.
 */
int networkInit() {
  // ----- Initialize Wi-Fi chip -----

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

  // ----- Host Setup -----

  // Enable station mode to be able to act as a client device.
  cyw43_arch_enable_sta_mode();

  // Set the device host name.
  setHostname();

  // ----- Connect to the Network -----

  if (wifiConnect() != 0) {
    return -1;
  }

  // Set Wi-Fi performance mode.
  while (cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM) != 0) {
    sleep_ms(100);
  };

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

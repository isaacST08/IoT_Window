#include <pico/stdio.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#include "ha_device.hh"
#include "lwip/apps/mqtt.h"
#include "pico/cyw43_arch.h"
#include "stepper_motor.hh"

extern "C" {
#include "network.h"
#include "pins.h"
}

/**
 * This is the main program start for the IoT window device.
 *
 * This will perform the tasks required to set up the device:
 * - Initialize IO.
 * - Initialize networking and MQTT.
 * - Initialize the device and hardware such as the stepper motor.
 * - Perform homing operations for the motor.
 *
 * Once setup is complete, the program goes into a main loop where
 * it performs repeated tasks such as checking the internet connection and
 * dispatching motor operations.
 */
int main() {
  /*
  **=========================================================================**
  ||                                                                         ||
  ||                ███████ ███████ ████████ ██    ██ ██████                 ||
  ||                ██      ██         ██    ██    ██ ██   ██                ||
  ||                ███████ █████      ██    ██    ██ ██████                 ||
  ||                     ██ ██         ██    ██    ██ ██                     ||
  ||                ███████ ███████    ██     ██████  ██                     ||
  ||                                                                         ||
  **=========================================================================**
  */

  // **=============================================**
  // ||          <<<<< GENERAL SETUP >>>>>          ||
  // **=============================================**

  // Initialize IO on the pico.
  stdio_init_all();
  sleep_ms(1000);

  // Once stdio has been initialized, announce the start of the program.
  printf("Program Start\n");
  stdio_flush();

  // Initialize the common pins (pins specific to a device get initialized with
  // that device).
  printf("Initializing Pins... ");
  init_pins();
  printf("done.\n");

  // Wave LEDs to visualize the start of the program without the need of a
  // console.
  {
    uint led_order[4] = {RED_LED_PIN, GREEN_LED_PIN, YELLOW_LED_PIN,
                         BLUE_LED_PIN};
    int ms = 100;

    // Wave 3 times.
    for (int i = 0; i < 3; i++) {
      for (uint pin : led_order) {
        gpio_put(pin, 1);
        sleep_ms(ms);
      }
      for (uint pin : led_order) {
        gpio_put(pin, 0);
        sleep_ms(ms);
      }
    }
  }

  // **=============================================**
  // ||          <<<<< NETWORK SETUP >>>>>          ||
  // **=============================================**

  // Initialize the network connection.
  printf("Initializing Networking... ");
  if (networkInit() != 0) {
    printf("Failed! Exiting.\n");
    stdio_flush();
    return -1;
  } else {
    printf("Success.\n");
  }

  // Print the memory size and the ring buffer size used by lwIP for debugging.
  if (MQTT_DEBUG)
    printf("MEMSIZE size: %d\nRINGBUF size: %d\n", MEM_SIZE,
           MQTT_OUTPUT_RINGBUF_SIZE);

  // ----- MQTT Setup -----
  printf("Setting up mqtt...\n");

  // Get lwIP locks while creating the MQTT client.
  cyw43_arch_lwip_begin();
  mqtt_client_t* mqtt_client = mqtt_client_new();
  cyw43_arch_lwip_end();

  // Attempt connection to the MQTT server and repeat until successful.
  printf("Insurring mqtt connection...\n");
  bool mqtt_setup_success;
  do {
    mqtt_setup_success = mqttDoConnect(mqtt_client);

    // Give MQTT subscribe requests that happen in the callback a chance to
    // complete.
    sleep_ms(1000);
  } while (!mqtt_setup_success);
  printf("MQTT setup finished.\n");

  // **============================================**
  // ||          <<<<< DEVICE SETUP >>>>>          ||
  // **============================================**

  // Initialize stepper motor for the window.
  stepper_motor::StepperMotor window_sm(SM_ENABLE_PIN, SM_DIR_PIN, SM_PULSE_PIN,
                                        SM_MS1_PIN, SM_MS2_PIN, MS_64, 1,
                                        mqtt_client);
  printf("Motor setup complete.\n");

  // Setup the Home Assistant device.
  haDeviceSetup(mqtt_client, &window_sm);

  // ----- WINDOW STEPPER MOTOR HOMING -----
  /*
   * Performs the homing operation for the window stepper motor.
   *
   * During the homing process the locks for lwIP and thus the network chip
   * itself are acquired. Even though control of the network chip is not
   * actively needed for this operation, the interrupts that occur due to the
   * network activity (specifically from the MQTT service) can cause problems
   * with the homing process. Problems range from stuttering during the motor
   * movement which can lead to an inaccurate reading of the home position and
   * unwanted noise, to the homing process getting interrupted and failing to
   * continue.
   *
   * Acquiring the locks simply makes the homing process smoother and more
   * reliable.
   *
   * Additionally, at prior to the motor being homed, any online device does not
   * know what state the window/motor is in and thus control of it via the
   * network isn't recommended anyways.
   *
   * For options related to the homing process such as the direction/side to
   * home to and which limit switch is on which side of the window, see the
   * `opts.h` file.
   */

  // Turn on the board led while the stepper motor is homing.
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

  // Enable the motor (turn it on).
  window_sm.enable();

  // Perform the homing operation.
  printf("Homeing...\n");
  window_sm.home();
  printf("Homeing Complete.\n");

  /*
  **=========================================================================**
  ||                                                                         ||
  ||  ███    ███  █████  ██ ███    ██     ██       ██████   ██████  ██████   ||
  ||  ████  ████ ██   ██ ██ ████   ██     ██      ██    ██ ██    ██ ██   ██  ||
  ||  ██ ████ ██ ███████ ██ ██ ██  ██     ██      ██    ██ ██    ██ ██████   ||
  ||  ██  ██  ██ ██   ██ ██ ██  ██ ██     ██      ██    ██ ██    ██ ██       ||
  ||  ██      ██ ██   ██ ██ ██   ████     ███████  ██████   ██████  ██       ||
  ||                                                                         ||
  **=========================================================================**
  */

  unsigned int loop_iteration = 0;
  while (true) {
    // If the network connection is down, set the red LED on and attempt to
    // reconnect.
    if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) !=
        CYW43_LINK_JOIN) {
      gpio_put(RED_LED_PIN, 1);

      // Leave the network.
      wifiDisconnect();

      // Reconnect to the network.
      wifiConnect();

      gpio_put(RED_LED_PIN, 0);
    }

    // ----- PROCESS STEPPER MOTOR ACTIONS -----
    /*
     * Requested actions are set during network interrupts such as when an
     * appropriate MQTT packet is received.
     *
     * To allow for further network interrupts to occur during motor movement,
     * instead of processing the request during the interrupt, the interrupt
     * simply sets the flag for the action the motor should perform and then
     * returns control to the regular schedule. Then, during each round of the
     * main loop, the requested operation is performed here.
     *
     * While this design results in slightly delayed (and sometimes variable)
     * response times from when a MQTT command is sent from a different device
     * to when the action actually starts being performed, it allows for the
     * actually network interrupt to take far less time to complete and then
     * makes the device become available to more network interrupts before the
     * motor operation has concluded.
     *
     * This design allows for one command to come in, for example, to open the
     * window, and then to still accept more commands while the first command is
     * being completed. One such command that is very useful is the STOP
     * command. Without this design, once a command is being executed it must
     * complete, but with it, the user can send an MQTT command to open the
     * window, and then half way through (or in the even of an emergency) call
     * the command to stop the window. This allows for undecided movement
     * (non-percentage positioning) or for emergency stops (assuming networking
     * is still operational).
     *
     * The action queue allows multiple actions to be queued sequentially.
     * Actions are processed in FIFO order - first action queued is first
     * executed. The queue holds up to 8 actions by default.
     */

    if (window_sm.hasQueuedActions()) {
      using namespace stepper_motor::action;

      // Get the next action and its argument from the queue.
      Action action = window_sm.action_queue.dequeue();
      // char action_arg[256];
      // window_sm.dequeueAction(&action, action_arg, sizeof(action_arg));

      // Perform appropriate operation.
      switch (action.action_type) {
        // Open window.
        case ActionType::OPEN: {
          printf("Open window operation activated (queue size: %d)\n",
                 window_sm.action_queue.getCount());
          bool open_success = window_sm.open();
          break;
        }

        // Close window.
        case ActionType::CLOSE: {
          printf("Close window operation activated (queue size: %d)\n",
                 window_sm.action_queue.getCount());
          bool close_success = window_sm.close();
          break;
        }

        // Move to percentage position.
        case ActionType::MOVE_TO_PERCENT: {
          // Parse the percentage value.
          float percentage = CLAMP(0.0, action.data.percent, 100.0);

          printf("Move to %f%% operation activated (queue size: %d)\n",
                 percentage, window_sm.action_queue.getCount());

          // Move to the requested position.
          window_sm.moveToPositionPercentage(percentage, true);

          break;
        }

        // Move to step position.
        case ActionType::MOVE_TO_STEP: {
          // Parse the step value.
          uint64_t step = MAX(0, action.data.step);

          printf("Move to step %llu operation activated (queue size: %d)\n",
                 step, window_sm.action_queue.getCount());

          // Move to the requested position.
          window_sm.moveToPosition(step, true);

          break;
        }

        // Do nothing.
        case ActionType::NONE:
          break;
      }

      // After the operation is completed, publish the new state of the device.
      // publishAll();
    }

    // Every ten thousand loop iterations, publish all the stepper motor data to
    // insure the server stays in sync.
    if (loop_iteration % 10000 == 0) {
      window_sm.publishAll();
    }

    // Blink the board led through each main loop cycle.
    // This helps show whether the main loop is continuing or if the program
    // might be stuck somewhere.
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(250);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(250);

    // Increment the loop iteration. It is fine if this wraps.
    loop_iteration += 1;
  }
}

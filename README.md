# IoT Window Project

This is a project creating an IoT (Internet of Things) window controller.

The project uses a Raspberry Pi Pico for the controller, a stepper motor, a
TMC2209 stepper motor driver, and a acme threaded rod for the physical movement
of the window, and 3D printed parts for mounting.

This device is designed to be used with Home assistant. On boot up of the
device, the device will send a HA device discovery packet that will
automatically configure the device in Home Assistant (when home assistant has
the MQTT integration set up to point to the same server you set up the IoT
window to connect to).

# Setup

Not included in the repo is the required `secrets.h` file that resides in the
`src` directory. To generate this file, use the command:

```
make secrets.h
```

If the file already exists, then nothing will happen, if it doesn't, then the
file will be created with all the required variables pre-filled with placeholder
values. Replace these values with the correct values for your network SSID and
password, as well as information for connecting to your MQTT server.

For manual creation of the `secrets.h` file, use the following template:

The file named `secrets.h` must be created in the `src` directory in order to
provide ones Wi-Fi credentials.

```
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "superCoolWiFiSSID"
#define WIFI_PASSWORD "superSecretPassword"

#define MQTT_SERVER "localhost"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt"
#define MQTT_PASSWORD "mqttUserPassword"

#endif
```

Insert your Wi-Fi SSID and password in the appropriate locations.

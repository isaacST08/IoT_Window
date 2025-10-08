# IoT Window Project

Custom firmware for an IoT window.

# Wi-Fi Credentials

A file named `secrets.h` must be created in the `src` directory in order to
provide ones Wi-Fi credentials.

Create the file following the template:

```
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "superCoolWiFiSSID"
#define WIFI_PASSWORD "superSecretPassword"

#endif
```

and insert your Wi-Fi SSID and password in the appropriate locations.

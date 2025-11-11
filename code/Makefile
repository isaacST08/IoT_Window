# Little driver makefile to make the complicated bootloader less of a hassle.

PROJECT=IoT_Window_V3
# PROJECT=blink

TOOLCHAIN_PREFIX=arm-none-eabi-

# Board in use. e.g. adafruit_feather_rp2040 would be another popular one.
BOARD=pico_w

build/$(PROJECT).uf2:

flash : build/$(PROJECT).uf2
	picotool load $<
	picotool reboot

build/$(PROJECT).uf2 build/$(PROJECT).elf: build FORCE
	$(MAKE) -C build
	$(TOOLCHAIN_PREFIX)size build/$(PROJECT).elf
	$(TOOLCHAIN_PREFIX)nm --print-size --size-sort --radix=d build/$(PROJECT).elf | awk '{printf("%5d %s\n", $$2, $$4);}' | sort -nr | head -20

disasm: build/$(PROJECT).elf
	$(TOOLCHAIN_PREFIX)objdump -C -S build/$(PROJECT).elf

build: secrets.h
	cmake -B build -DCMAKE_VERBOSE_MAKEFILE=ON -DPICO_BOARD=$(BOARD)

secrets.h:
	if [ ! -f src/secrets.h ]; then \
		echo "Missing secrets file! Generating..."; \
		echo "#ifndef SECRETS_H" >> src/secrets.h; \
		echo "#define SECRETS_H" >> src/secrets.h; \
		echo "" >> src/secrets.h; \
		echo "#define WIFI_SSID \"myWiFiSSID\"" >> src/secrets.h; \
		echo "#define WIFI_PASSWORD \"superSecretPassword\"" >> src/secrets.h; \
		echo "" >> src/secrets.h; \
		echo "#define MQTT_SERVER \"localhost\"" >> src/secrets.h; \
		echo "#define MQTT_PORT 1883" >> src/secrets.h; \
		echo "#define MQTT_USER \"hass\"" >> src/secrets.h; \
		echo "#define MQTT_PASSWORD \"hassPassword\"" >> src/secrets.h; \
		echo "" >> src/secrets.h; \
		echo "#endif" >> src/secrets.h; \
	fi

clean:
	rm -rf build

FORCE:

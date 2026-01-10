cmake_minimum_required(VERSION 3.25)

# Need to include pico-sdk cmake support. Must happen before project.
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(IoT_Window_V3
  LANGUAGES C CXX ASM
  VERSION 1.0
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # Get compilation DB

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(src/stepper_motor)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
    src/main.cc
)

# pico_add_library(network NOFLAG)
# target_sources(network INTERFACE
#   ${CMAKE_CURRENT_LIST_DIR}/src/network.c
# )
add_library(options INTERFACE src/common.hh src/opts.hh src/advanced_opts.hh)
target_include_directories(options INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)
# Network
add_library(network src/network.hh src/network.cc)
target_link_libraries(network pico_stdlib pico_cyw43_arch_lwip_threadsafe_background pico_lwip_mqtt)
target_include_directories(network PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/src)
# Pins
add_library(pins src/pins.hh src/pins.cc)
target_link_libraries(pins pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)
target_include_directories(pins PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/src)
# HA Device
add_library(ha_device src/ha_device.hh src/ha_device.cc)
target_link_libraries( ha_device 
  pico_stdlib
  pico_cyw43_arch_lwip_threadsafe_background
  pico_lwip_mqtt
  pins
  stepper_motor
)
target_include_directories(ha_device PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/src)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
  pico_stdlib
  # pico_cyw43_arch_none
  pico_cyw43_arch_lwip_threadsafe_background
  pico_sync
  pico_lwip_mqtt
  network
  pins
  action_queue
  stepper_motor
  ha_device
)

target_include_directories(${PROJECT_NAME} PRIVATE 
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/src
)

# Choice of available stdio outputs
pico_enable_stdio_usb(${PROJECT_NAME} 1)  # ~13880 extra bytes
pico_enable_stdio_uart(${PROJECT_NAME} 0) # ~1176 extra bytes


# **============================================**
# ||          <<<<< ACTION QUEUE >>>>>          ||
# **============================================**

add_library( action_queue
  action_queue.hh
  action_queue.cc
)

target_link_libraries( action_queue 
  pico_stdlib 
  pico_cyw43_arch_lwip_threadsafe_background
  pico_sync
  pins
  options
)

target_include_directories(
  action_queue 
  PRIVATE 
  ${CMAKE_CURRENT_LIST_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}
)


# **=============================================**
# ||          <<<<< STEPPER MOTOR >>>>>          ||
# **=============================================**

add_library( stepper_motor
  stepper_motor.hh
  stepper_motor.cc
)

target_link_libraries( stepper_motor 
  pico_stdlib 
  pico_cyw43_arch_lwip_threadsafe_background 
  action_queue 
  pins
  options
)

target_include_directories( 
  stepper_motor 
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}
)


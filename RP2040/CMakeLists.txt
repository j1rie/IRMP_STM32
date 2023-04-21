cmake_minimum_required(VERSION 3.12)

include(../pico-sdk/pico_sdk_init.cmake)

string(TIMESTAMP BUILD_TIME "%Y-%m-%d_%H-%M")
# sets PROJECT_NAME and PROJECT_SOURCE_DIR
project(${BUILD_TIME}_RP2040_hid_irmp)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    src/main.c
    usb/usb_descriptors.c
    irmp/irmp.c
    irmp/irsnd.c
    src/irmpmain.c
    eeprom/eeprom.c
    src/usb_hid.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/irmp
        ${CMAKE_CURRENT_SOURCE_DIR}/usb
        ${CMAKE_CURRENT_SOURCE_DIR}/eeprom
        ${CMAKE_CURRENT_SOURCE_DIR}/timestamp
)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(${PROJECT_NAME} pico_stdlib tinyusb_device tinyusb_board hardware_pwm hardware_flash hardware_exception timestamp)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
# for compability with early revisions
target_compile_definitions(${PROJECT_NAME} PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(${PROJECT_NAME})

add_library(timestamp OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/timestamp/timestamp.c)

execute_process (COMMAND bash -c "grep -e 'Version IRMP' ${CMAKE_CURRENT_SOURCE_DIR}/irmp/README.txt | awk '{print $$3}'" OUTPUT_VARIABLE IRMP_VERSION)

string(STRIP ${IRMP_VERSION} IRMP_VERSION)

target_compile_definitions(timestamp PRIVATE FW_STR="${BUILD_TIME}_RP2040   IRMP-Version: ${IRMP_VERSION}*RP2040")

set(FW_STR "${BUILD_TIME}_RP2040   IRMP-Version: ${IRMP_VERSION}*RP2040")

target_include_directories(timestamp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_custom_target(clear_cache COMMAND ${CMAKE_COMMAND} -U BUILD_TIME ${CMAKE_BINARY_DIR})

add_dependencies(timestamp clear_cache)

message(STATUS "BUILD_TIME=${BUILD_TIME}")
message(STATUS "IRMP_VERSION=${IRMP_VERSION}")
message(STATUS "FW_STR=${FW_STR}")
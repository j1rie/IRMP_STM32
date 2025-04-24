cmake_minimum_required(VERSION 3.13)

include(../pico-sdk/pico_sdk_init.cmake)

string(TIMESTAMP BUILD_TIME "%Y-%m-%d_%H-%M")
# sets PROJECT_NAME and PROJECT_SOURCE_DIR
project(${BUILD_TIME}_${PICO_BOARD}_hid_irmp)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    src/main.c
    usb/usb_descriptors.c
    irmp/irmp.c
    irmp/irsnd.c
    src/irmpmain.c
    eeprom/eeprom.c
    src/usb_hid.c
    src/ws2812.c
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/irmp
        ${CMAKE_CURRENT_SOURCE_DIR}/usb
        ${CMAKE_CURRENT_SOURCE_DIR}/eeprom
        ${CMAKE_CURRENT_SOURCE_DIR}/timestamp
)

target_link_libraries(${PROJECT_NAME} pico_stdlib tinyusb_device tinyusb_board hardware_pwm hardware_flash hardware_exception timestamp hardware_pio)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
# for compability with early revisions
target_compile_definitions(${PROJECT_NAME} PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(${PROJECT_NAME})

add_library(timestamp OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/timestamp/timestamp.c)

execute_process (COMMAND bash -c "grep -e 'Version IRMP' ${CMAKE_CURRENT_SOURCE_DIR}/irmp/README.txt | awk '{print $$3}'" OUTPUT_VARIABLE IRMP_VERSION)

string(STRIP ${IRMP_VERSION} IRMP_VERSION)

target_compile_definitions(timestamp PRIVATE FW_STR="${PROJECT_NAME}   IRMP-Version: ${IRMP_VERSION}*RP2xxx")

target_include_directories(timestamp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_custom_target(clear_cache COMMAND ${CMAKE_COMMAND} -U BUILD_TIME ${CMAKE_BINARY_DIR})

add_dependencies(timestamp clear_cache)

message(STATUS "FW_STR=${FW_STR}")

target_compile_definitions(${PROJECT_NAME} PUBLIC PICO_USE_FASTEST_SUPPORTED_CLOCK=1)

cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico_w)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

set(project_name "wifi_ssid_password" C CXX)
project(${project_name})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(../.. pico_flash_param)
add_subdirectory(../lib/picow_config_wifi picow_config_wifi)

set(bin_name ${PROJECT_NAME})
add_executable(${bin_name}
    main.cpp
)

target_link_libraries(${bin_name}
    hardware_adc
    pico_stdlib
    pico_flash_param
    picow_config_wifi
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${bin_name})

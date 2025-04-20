cmake_minimum_required(VERSION 3.13)

# define "w" board to use cyw43_arch_xxx() function
if(PICO_BOARD)
    unset(PICO_BOARD)
endif()
if(PICO_PLATFORM STREQUAL rp2350)
    set(PICO_BOARD pico2_w)
else()
    set(PICO_BOARD pico_w)
endif()


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

target_include_directories(picow_config_wifi INTERFACE
    $ENV{PICO_EXAMPLES_PATH}/pico_w/wifi
    $ENV{PICO_EXAMPLES_PATH}/pico_w/wifi/ntp_client
)

target_link_libraries(${bin_name}
    hardware_adc
    pico_cyw43_arch_lwip_poll
    pico_flash_param
    pico_stdlib
    picow_config_wifi
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${bin_name})

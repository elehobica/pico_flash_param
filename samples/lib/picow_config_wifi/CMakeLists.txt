if (NOT TARGET picow_config_wifi)
    add_library(picow_config_wifi INTERFACE)

    target_sources(picow_config_wifi INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/config_wifi.cpp
    )

    target_include_directories(picow_config_wifi INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        $ENV{PICO_EXAMPLES_PATH}/pico_w/wifi
        $ENV{PICO_EXAMPLES_PATH}/pico_w/wifi/ntp_client
    )

    target_link_libraries(picow_config_wifi INTERFACE
        pico_stdlib
        pico_cyw43_arch_lwip_poll
    )
endif()

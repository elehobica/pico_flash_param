if (NOT TARGET picow_config_wifi)
    add_library(picow_config_wifi INTERFACE)

    target_sources(picow_config_wifi INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/config_wifi.cpp
    )

    target_include_directories(picow_config_wifi INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
    )

    target_link_libraries(picow_config_wifi INTERFACE
        pico_stdlib
    )
endif()

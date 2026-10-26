# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

# FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

# idf_component_register(SRCS ${app_sources})

file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/app/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/engine/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/indication/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/input/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/lighting/*.cpp"
)

idf_component_register(
    SRCS
        ${APP_SOURCES}
        "platform/espidf/main.cpp"
        "platform/espidf/drivers/PwmLed.cpp"
        "platform/espidf/drivers/LedcResourceManager.cpp"
        "platform/espidf/drivers/PwmMotor.cpp"
        "platform/espidf/drivers/Button.cpp"
        "platform/espidf/drivers/Ky040Encoder.cpp"
        "platform/espidf/drivers/RmtRgbLed.cpp"
        "platform/espidf/drivers/Photoresistor.cpp"
        "platform/espidf/bootstrap/ApplicationBootstrap.cpp"
    INCLUDE_DIRS
        "../include"
    REQUIRES
        esp_driver_gpio
        esp_driver_ledc
        esp_adc
        esp_psram
        led_strip
    PRIV_INCLUDE_DIRS
        "platform/espidf"
)

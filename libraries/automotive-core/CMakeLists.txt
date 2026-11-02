add_library(automotive-core STATIC
    src/app/Application.cpp
    src/app/AmbientLightController.cpp
    src/app/CarController.cpp
    src/engine/EngineController.cpp
    src/indication/StatusIndicatorController.cpp
    src/lighting/BrightnessAnimator.cpp
    src/lighting/LightingController.cpp
)

target_include_directories(automotive-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(automotive-core
    PUBLIC embedded-input
)

target_compile_features(automotive-core
    PUBLIC cxx_std_17
)


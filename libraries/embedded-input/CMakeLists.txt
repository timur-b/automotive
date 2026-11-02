add_library(embedded-input STATIC
    src/input/Debouncer.cpp
    src/input/ButtonGestureRecognizer.cpp
    src/input/ButtonInputManager.cpp
    src/input/QuadratureDecoder.cpp
)

target_include_directories(embedded-input
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(embedded-input
    PUBLIC cxx_std_17
)


add_executable(automotive-host-test
    main.cpp
)

target_link_libraries(automotive-host-test
    PRIVATE
        automotive-core
        embedded-input
)


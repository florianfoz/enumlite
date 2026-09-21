add_executable(enumlite_tests
    test_enumlite.cpp
)

target_link_libraries(enumlite_tests
    PRIVATE
        enumlite::enumlite
)

add_test(
    NAME enumlite_tests
    COMMAND enumlite_tests
)
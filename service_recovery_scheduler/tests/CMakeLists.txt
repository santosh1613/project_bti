include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# Prevent GoogleTest from overriding the parent project's compiler/linker
# settings on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    test_policy.cpp
    test_service.cpp
    test_scheduler.cpp
)

target_link_libraries(unit_tests PRIVATE
    service_recovery_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

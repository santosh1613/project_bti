cmake_minimum_required(VERSION 3.16)

project(service_recovery_scheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

###############################################################################
# Core library (all logic except main())
###############################################################################

add_library(service_recovery_lib
    src/Action.cpp
    src/Policy.cpp
    src/Scheduler.cpp
    src/Service.cpp
)

target_include_directories(service_recovery_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

###############################################################################
# Application executable
###############################################################################

add_executable(service_recovery_scheduler
    src/main.cpp
)

target_link_libraries(service_recovery_scheduler PRIVATE service_recovery_lib)

###############################################################################
# Tests (GoogleTest via FetchContent)
###############################################################################

option(BUILD_TESTING "Build the unit tests" ON)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    enable_testing()
    add_subdirectory(tests)
endif()

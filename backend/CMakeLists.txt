cmake_minimum_required(VERSION 3.16)
project(ReconGraph VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(RECONGRAPH_BUILD_TESTS "Build tests" OFF)

# Fetch cpp-httplib (header-only)
include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.18.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(httplib)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../dsa
)

# Source files (Phase 1 - minimal)
set(RECONGRAPH_SOURCES
    src/main.cpp
    src/api/server.cpp
)

# Executable
add_executable(recongraph_server ${RECONGRAPH_SOURCES})

target_link_libraries(recongraph_server PRIVATE httplib::httplib)

# Platform-specific notes
if(WIN32)
    target_link_libraries(recongraph_server PRIVATE ws2_32)
endif()

# Install
install(TARGETS recongraph_server RUNTIME DESTINATION bin)

message(STATUS "ReconGraph backend configured (Phase 1 foundation)")

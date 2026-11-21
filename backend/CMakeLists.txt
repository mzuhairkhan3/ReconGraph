cmake_minimum_required(VERSION 3.16)
project(ReconGraph VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# Source files – Phase 2
set(RECONGRAPH_SOURCES
    src/main.cpp
    src/api/server.cpp
    src/core/url_normalizer.cpp
    src/graph/attack_surface_graph.cpp
    src/crawler/recon_collector.cpp
)

add_executable(recongraph_server ${RECONGRAPH_SOURCES})

target_link_libraries(recongraph_server PRIVATE httplib::httplib)

if(WIN32)
    target_link_libraries(recongraph_server PRIVATE ws2_32)
endif()

# On some platforms regex + threads may need explicit linking
find_package(Threads REQUIRED)
target_link_libraries(recongraph_server PRIVATE Threads::Threads)

install(TARGETS recongraph_server RUNTIME DESTINATION bin)

message(STATUS "ReconGraph backend configured (Phase 2 – Core Engine)")

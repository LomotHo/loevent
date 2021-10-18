cmake_minimum_required(VERSION 3.14)
project(loevent)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
set(CMAKE_EXPORT_COMPILECOMMANDS ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)

find_package(spdlog REQUIRED)

# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/mt)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/bench)

# aux_source_directory(src/core/ core)

# SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
# SET(LOEVENT_SRC "src/tmp.cpp")

# include_directories("${PROJECT_SOURCE_DIR}/inc")
# ADD_LIBRARY(loevent STATIC ${LOEVENT_SRC})

add_executable(pp_client example/pp_client.cpp)
target_link_libraries(pp_client spdlog::spdlog)

add_executable(echo_server example/echo_server.cpp)
target_link_libraries(echo_server spdlog::spdlog)

add_executable(http_server example/http_server.cpp)
target_link_libraries(http_server spdlog::spdlog)

add_executable(bench_cli example/bench_cli.cpp)
target_link_libraries(bench_cli spdlog::spdlog)

add_executable(cmd_client example/cmd_client.cpp)
target_link_libraries(cmd_client)

add_executable(demo example/demo.cpp)
target_link_libraries(demo spdlog::spdlog)

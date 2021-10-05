cmake_minimum_required(VERSION 3.14)
project(reactor-demo)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)

find_package(spdlog REQUIRED)

# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/mt)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/bench)

# aux_source_directory(src/core/ core)

add_executable(pp_client src/pingpong/client.cpp)
target_link_libraries(pp_client spdlog::spdlog)

add_executable(pp_server src/pingpong/server.cpp)
target_link_libraries(pp_server spdlog::spdlog)

add_executable(benchmark_client src/benchmark/client.cpp)
target_link_libraries(benchmark_client spdlog::spdlog)

add_executable(cmd_client src/cmd_client.cpp)
target_link_libraries(cmd_client)

# add_executable(cb src/cb.cpp)
# target_link_libraries(cb spdlog::spdlog pthread rt)

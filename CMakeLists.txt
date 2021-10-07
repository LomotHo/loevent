cmake_minimum_required(VERSION 3.14)
project(reactor-demo)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
set(CMAKE_EXPORT_COMPILECOMMANDS ON)

include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)

find_package(spdlog REQUIRED)

# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/mt)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/bench)

# aux_source_directory(src/core/ core)

add_executable(pp_client src/pp_client.cpp)
target_link_libraries(pp_client spdlog::spdlog)

add_executable(echo_server src/echo_server.cpp)
target_link_libraries(echo_server spdlog::spdlog)

add_executable(bench_cli src/bench_cli.cpp)
target_link_libraries(bench_cli spdlog::spdlog)

add_executable(cmd_client src/cmd_client.cpp)
target_link_libraries(cmd_client)

# add_executable(cb src/cb.cpp)
# target_link_libraries(cb spdlog::spdlog pthread rt)

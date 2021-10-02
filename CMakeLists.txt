cmake_minimum_required(VERSION 3.14)
project(reactor-demo)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)

find_package(spdlog REQUIRED)

# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/mt)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/bench)

# aux_source_directory(src/core/ core)

add_executable(client src/client.cpp)
target_link_libraries(client spdlog::spdlog pthread rt)

add_executable(server src/server.cpp)
target_link_libraries(server spdlog::spdlog)

add_executable(client-c src/client-c.cpp)
target_link_libraries(client-c)

# add_executable(cb src/cb.cpp)
# target_link_libraries(cb spdlog::spdlog pthread rt)

cmake_minimum_required(VERSION 3.14)
project(reactor-demo)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)

find_package(spdlog REQUIRED)

# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/mt)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/bench)


# aux_source_directory(src/core/ core)

# add_executable(server src/main.cpp ${core})
# target_link_libraries(server spdlog::spdlog muduo_net muduo_base pthread rt)

# add_executable(testCli ${core} src/client/testCli.cpp)
# target_link_libraries(testCli spdlog::spdlog muduo_net muduo_base pthread rt)

# add_executable(client src/client/demo0.cpp)
# target_link_libraries(client spdlog::spdlog muduo_net muduo_base pthread rt)

add_executable(main src/main.cpp)
target_link_libraries(main spdlog::spdlog pthread rt)


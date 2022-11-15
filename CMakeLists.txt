cmake_minimum_required(VERSION 3.22.1)

project(grt-server)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(grt/Base)
add_subdirectory(grt/Http)
add_subdirectory(grt/Log)
add_subdirectory(grt/Net)

target_link_libraries(${PROJECT_NAME} grt_net_lib grt_log_lib grt_http_lib grt_base_lib)
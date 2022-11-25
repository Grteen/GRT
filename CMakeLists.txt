cmake_minimum_required(VERSION 3.22.1)

project(grt-server)

add_definitions("-Wall -g")

add_executable(${PROJECT_NAME} signup_in.cpp)

add_subdirectory(grt/Base)
add_subdirectory(grt/Http)
add_subdirectory(grt/Log)
add_subdirectory(grt/Net)

target_link_libraries(${PROJECT_NAME} grt_net_lib grt_log_lib grt_http_lib grt_base_lib)

# <---------- set c++ standard ------------->
# NOTE: you must build redis-plus-plus and your application code with the same standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(grt-server PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(grt-server ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(grt-server PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(grt-server ${REDIS_PLUS_PLUS_LIB})

find_library(MYSQL mysqlcppconn)
target_link_libraries(grt-server ${MYSQL})
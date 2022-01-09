cmake_minimum_required(VERSION 3.11)

project(engi_threadpool LANGUAGES C)

include_directories(engi_common/)

add_subdirectory(engi_thread)
add_subdirectory(engi_queue)

set(CMAKE_C_FLAGS_DEBUG "-std=gnu2x -Og -Wall -Wextra -g3 -gdwarf-3 -fanalyzer -flto")
set(CMAKE_C_FLAGS_RELEASE "-std=gnu2x -Ofast -flto")

set(src ${PROJECT_NAME}.c)

set(object_library ${PROJECT_NAME}_object)
set(static_library ${PROJECT_NAME}_static)
set(shared_library ${PROJECT_NAME}_shared)

add_library(object_library OBJECT ${src})
set_property(TARGET object_library PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(static_library STATIC $<TARGET_OBJECTS:object_library>)
set_property(TARGET static_library PROPERTY OUTPUT_NAME ${PROJECT_NAME})

add_library(shared_library SHARED $<TARGET_OBJECTS:object_library>)
set_property(TARGET shared_library PROPERTY OUTPUT_NAME ${PROJECT_NAME})

add_executable(test main.c)
target_link_libraries(test shared_library)

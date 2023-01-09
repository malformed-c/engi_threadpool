cmake_minimum_required(VERSION 3.11)
include_guard(GLOBAL)

project(engi_threadpool LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/../engi_common/ engi_thread/ engi_queue/)

add_subdirectory(engi_thread/)
add_subdirectory(engi_queue/)

set(CMAKE_C_FLAGS_DEBUG "-std=gnu2x -Og -Wall -Wextra -g3 -gdwarf-3 -fanalyzer -flto")
set(CMAKE_C_FLAGS_RELEASE "-std=gnu2x -Ofast -flto")

set(src ${PROJECT_NAME}.c)

set(object_library ${PROJECT_NAME}_object)
set(static_library ${PROJECT_NAME}_static)
set(shared_library ${PROJECT_NAME}_shared)

add_library(${object_library} OBJECT ${src})
set_property(TARGET ${object_library} PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(${static_library} STATIC $<TARGET_OBJECTS:${object_library}>)
set_property(TARGET ${static_library} PROPERTY OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${static_library} engi_thread_static -static engi_queue_static -static pthread -static)


add_library(${shared_library} SHARED $<TARGET_OBJECTS:${object_library}>)
set_property(TARGET ${shared_library} PROPERTY OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${shared_library} engi_thread_shared -shared engi_queue_shared -shared pthread -shared)


add_executable(${PROJECT_NAME}_test main.c)
#set_property(TARGET ${PROJECT_NAME}_test PROPERTY POSITION_INDEPENDENT_CODE 1)
target_link_libraries(${PROJECT_NAME}_test -static engi_thread_static -static engi_queue_static -static ${static_library} -static pthread -static)
#target_link_libraries(${PROJECT_NAME}_test -shared engi_thread_shared -shared engi_queue_shared -shared ${shared_library} -shared pthread -shared)

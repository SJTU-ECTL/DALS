cmake_minimum_required(VERSION 3.9)
project(abc-plus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    include_directories(/usr/local/include)
endif ()

set(abc_include ${PROJECT_SOURCE_DIR}/abc/src/)
set(abc_include ${PROJECT_SOURCE_DIR}/abc/src/ PARENT_SCOPE)
set(abc_plus_include ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(abc_plus_include ${CMAKE_CURRENT_SOURCE_DIR}/include/ PARENT_SCOPE)

add_definitions(-Wall -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-but-set-variable)

include_directories(${abc_include})
include_directories(${abc_plus_include})
find_package(Boost REQUIRED COMPONENTS regex system filesystem timer)
file(GLOB abc_plus_src_files
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(abc_plus STATIC ${abc_plus_src_files})
target_link_libraries(abc_plus
        libabc dl pthread readline
        ${Boost_LIBRARIES})

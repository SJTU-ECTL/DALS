cmake_minimum_required(VERSION 3.9)
project(dals)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    include_directories(/usr/local/include)
endif ()

set(ABC_USE_NAMESPACE abc)
set(dals_include ${PROJECT_SOURCE_DIR}/include/)
add_subdirectory(abc EXCLUDE_FROM_ALL)
add_subdirectory(abc-plus)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
add_definitions(-Wall -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-but-set-variable)
find_package(Boost REQUIRED COMPONENTS regex system filesystem timer)
include_directories(${abc_plus_include})
include_directories(${dals_include})
file(GLOB dals_src_files "include/*.h" "src/*.cpp")
add_executable(dals ${dals_src_files})
target_link_libraries(dals
        abc_plus
        ${Boost_LIBRARIES})

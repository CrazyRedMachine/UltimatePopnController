cmake_minimum_required(VERSION 3.14)
project(popn_modeswitch)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

include_directories(${popn_modeswitch_SOURCE_DIR})
link_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(modeswitch modeswitch.cpp)
cmake_minimum_required(VERSION 3.14)
project(popn_modeswitch)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

include_directories(${popn_modeswitch_SOURCE_DIR})
link_directories(${PROJECT_SOURCE_DIR}/include)

# STATIC LINK HID
find_library(HIDDLL NAMES hid PATHS ${CMAKE_SOURCE_DIR}/lib)
message(STATUS "find_library for hid returned ${HIDDLL}")
add_library(hid MODULE IMPORTED)
set_property(TARGET hid PROPERTY IMPORTED_LOCATION "${HIDDLL}")

add_executable(modeswitch modeswitch.cpp)
target_link_libraries(modeswitch hid)
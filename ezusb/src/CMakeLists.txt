cmake_minimum_required(VERSION 3.14)
project(UltimatePopnController-ezusb)

set(CMAKE_CXX_STANDARD 14)

include_directories(${ezusb_SOURCE_DIR})
link_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_library(ezusb SHARED ezusb.cpp ezusb.h)
target_link_libraries(ezusb hid)
SET_TARGET_PROPERTIES(ezusb PROPERTIES PREFIX "")

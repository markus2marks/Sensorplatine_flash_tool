cmake_minimum_required(VERSION 2.6)
project(ProjectName)
find_package(Curses REQUIRED)

#include_directories("include")

include_directories(${CURSES_INCLUDE_DIR})

add_library(app net_com.cpp tftp_client.cpp tftp_packet.cpp)

add_executable(main main.cpp)

target_link_libraries(main app ${CURSES_LIBRARIES})


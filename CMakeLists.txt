# Copyright (C) 2019 Martin Lambers <marlam@marlam.de>
#
# Copying and distribution of this file, with or without modification, are
# permitted in any medium without royalty provided the copyright notice and this
# notice are preserved. This file is offered as-is, without any warranty.

cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

project(mucalc)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(MUPARSER REQUIRED)
find_package(READLINE REQUIRED)

include_directories(${MUPARSER_INCLUDE_DIRS} ${READLINE_INCLUDE_DIRS})
link_directories(${MUPARSER_LIBRARY_DIRS} ${READLINE_LIBRARY_DIRS})
add_executable(mucalc mucalc.cpp)
target_link_libraries(mucalc ${MUPARSER_LIBRARIES} ${READLINE_LIBRARIES})
install(TARGETS mucalc RUNTIME DESTINATION bin)

#
# Main cmake configuration for CerQall
#
#  Copyright (c) 2018, Arthur Wisz
#  All rights reserved.
#
# See the LICENSE file for the license terms and conditions.
#

project(CerQall)

cmake_minimum_required(VERSION 3.1.0)

install(DIRECTORY include/cercall/qt DESTINATION include/cercall)
install(FILES LICENSE LICENSE.APACHEv2 LICENSE.LGPLv3 DESTINATION include/cercall/qt)

find_path(CERCALL_INCLUDE_DIR NAMES cercall/cercall.h PATHS ${CMAKE_SOURCE_DIR}/../cercall/include)

if(EXISTS "${CERCALL_INCLUDE_DIR}")
    message(STATUS "CerCall library found in ${CERCALL_INCLUDE_DIR}")
    include_directories(AFTER ${CERCALL_INCLUDE_DIR})
else(EXISTS "${CERCALL_INCLUDE_DIR}")
    message(FATAL_ERROR "CerCall library could not be found.")
endif(EXISTS "${CERCALL_INCLUDE_DIR}")

find_path(CEREAL_INCLUDE_DIR NAMES cereal/cereal.hpp PATHS ${CMAKE_SOURCE_DIR}/../cereal/include)

if(EXISTS "${CEREAL_INCLUDE_DIR}")
    message(STATUS "cereal library found in ${CEREAL_INCLUDE_DIR}")
    include_directories(AFTER ${CEREAL_INCLUDE_DIR})
    add_definitions(-DHAS_CEREAL)
else(EXISTS "${CEREAL_INCLUDE_DIR}")
    message(FATAL_ERROR "cereal library (cereal.hpp) could not be found.")
endif(EXISTS "${CEREAL_INCLUDE_DIR}")

find_package(Qt5 REQUIRED COMPONENTS Core Network)

find_package(Threads)

set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall -g -Wextra -Wshadow -ftemplate-backtrace-limit=64 -Wno-implicit-fallthrough ${CMAKE_CXX_FLAGS}")

add_subdirectory(examples)

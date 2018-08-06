# CMakeLists for mwtagger
#
# Copyright 2016-2018 by Damir Cavar (http://damir.cavar.me/)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


cmake_minimum_required(VERSION 3.2.2)
project(mwtagger)
set(EXECUTABLE_NAME "mwtagger")

# add all .cpp and .h files to source
file(GLOB ALL_MY_SRC
    "*.h"
    "*.cpp")

set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE) # setting two custom configuration types
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# add Foma libs for morphological analysis parsing
# find the dynamic library
# find_library(FomaLib NAMES foma PATHS /usr/local/lib)
# find the static library
find_library(FomaLib NAMES libfoma.a)
# Foma requires libz
find_library(ZLib NAMES z)

# add Boost libs
# set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS    OFF)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK       ON)
SET(BOOST_MIN_VERSION        "1.47.0")
find_package(Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS program_options filesystem system)
if (NOT Boost_FOUND)
      message(FATAL_ERROR "Fatal error: Boost (version >= ${BOOST_MIN_VERSION}) required.\n")
endif (NOT Boost_FOUND)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(${EXECUTABLE_NAME} ${ALL_MY_SRC})
target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES} ${FomaLib} ${ZLib})

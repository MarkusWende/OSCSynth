cmake_minimum_required (VERSION 3.6)
project(OSCSynth)

# We are using C++ 14 and will make use of C++ exceptions.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -Wall -Wextra -Wpedantic -Wno-deprecated -Wno-variadic-macros")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D__OSCSYNTH_DEBUG__)
endif ()

set(THIRD_PARTY_DIR "external")
set(MAIN_SOURCE_DIR "src")
set(MAIN_INCLUDE_DIR "include")
set(MAIN_LIB_DIR "lib")
include_directories(${MAIN_INCLUDE_DIR})

# RtMidi
set(RTMIDI_DIR ${THIRD_PARTY_DIR}/rtmidi)
add_subdirectory(${RTMIDI_DIR} ${PROJECT_BINARY_DIR}/rtmidi.dir)
include_directories(${RTMIDI_DIR})

# liblo
set(LIBLO_DIR ${THIRD_PARTY_DIR}/liblo)
include_directories(${LIBLO_DIR}/build)
include_directories(${LIBLO_DIR})

# jackcpp
set(JACKCPP_DIR ${THIRD_PARTY_DIR}/jackcpp)
include_directories(${JACKCPP_DIR}/include)

# jackaudio
set(JACKAUDIO_DIR ${THIRD_PARTY_DIR}/jackaudio)
include_directories(${JACKAUDIO_DIR})

# AixLOG
set(AIXLOG_DIR ${THIRD_PARTY_DIR}/aixlog)
include_directories(${AIXLOG_DIR}/include)

add_library(
    app
    ${MAIN_SOURCE_DIR}/adsr.cpp
    ${MAIN_SOURCE_DIR}/Biquad.cpp
    ${MAIN_SOURCE_DIR}/distortion.cpp
    ${MAIN_SOURCE_DIR}/midiman.cpp
    ${MAIN_SOURCE_DIR}/noise.cpp
    ${MAIN_SOURCE_DIR}/osc_synth.cpp
    ${MAIN_SOURCE_DIR}/oscicontainer.cpp
    ${MAIN_SOURCE_DIR}/oscman.cpp
    ${MAIN_SOURCE_DIR}/releaseNote.cpp
    ${MAIN_SOURCE_DIR}/sawtoothwave.cpp
    ${MAIN_SOURCE_DIR}/sinusoid.cpp
    ${MAIN_SOURCE_DIR}/squarewave.cpp
)

add_library(
    jackcpp
    ${JACKCPP_DIR}/include/jackaudioio.hpp
    ${JACKCPP_DIR}/include/jackblockingaudioio.hpp
    ${JACKCPP_DIR}/include/jackmidiport.hpp
    ${JACKCPP_DIR}/include/jackringbuffer.hpp
    ${JACKCPP_DIR}/src/jackaudioio.cpp
    ${JACKCPP_DIR}/src/jackblockingaudioio.cpp
    ${JACKCPP_DIR}/src/jackmidiport.cpp
)

add_library(liblo SHARED IMPORTED)
set_property(TARGET liblo PROPERTY IMPORTED_LOCATION "../${LIBLO_DIR}/build/liblo.so")

# The 'oscsynth' executable
add_executable(
    oscsynth
    ${MAIN_SOURCE_DIR}/main.cpp
)

target_link_libraries(oscsynth app rtmidi jackcpp liblo jack)
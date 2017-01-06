cmake_minimum_required(VERSION 2.8)
set(CMAKE_SYSTEM_NAME Windows)
project(dix C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES dix modules/frame modules/text)
add_library(dix ${SOURCE_FILES})
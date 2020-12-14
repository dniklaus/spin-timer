# Spin-Timer Lib
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(PROJECT "SpinTimer")
project(${PROJECT} VERSION 3.0.0 LANGUAGES CXX)

# Include own cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(Documentation)

# Names of needed components
set(TARGET ${PROJECT})
set(INCLUDE_DIRECTORIES ".")
set(SOURCES
	"SpinTimer.cpp"
	"SpinTimerContext.cpp"
	"UptimeInfo.cpp"
)

# Make the library
add_library(${TARGET} OBJECT ${SOURCES})
target_link_libraries(${TARGET})
target_include_directories(${TARGET} PUBLIC ${INCLUDE_DIRECTORIES})

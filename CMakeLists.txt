# Spin-Timer Lib
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(PROJECT "SpinTimer")
project(${PROJECT} VERSION 3.0.0 LANGUAGES CXX)

set(TARGET ${PROJECT})
set(INCLUDE_DIRECTORIES ".")
set(SOURCES
	"SpinTimer.cpp"
	"SpinTimerContext.cpp"
	"UptimeInfo.cpp"
)

add_library(${TARGET} OBJECT ${SOURCES})
target_link_libraries(${TARGET})
target_include_directories(${TARGET} PUBLIC ${INCLUDE_DIRECTORIES})

cmake_minimum_required(VERSION 3.16)
project(wiring-timer)

add_library(${PROJECT_NAME} STATIC 
    Timer.cpp
    TimerContext.cpp
    UptimeInfo.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC . )

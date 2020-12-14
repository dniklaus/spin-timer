# Spin Timer Unit Tests
cmake_minimum_required(VERSION 3.16)

set(PROJECT spin-timer-test)
project(${PROJECT} LANGUAGES CXX)

include(GoogleTest)
enable_testing()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Add the spin timer library to our build.
add_subdirectory("../" ${CMAKE_CURRENT_BINARY_DIR}/SpinTimer)

# Set names of needed build components
set(TARGET ${PROJECT})
set(SOURCES 
  "main.cpp"
  "Test_SpinTimer.cpp"  
)
set(INCLUDE_DIRECTORIES 
  "."
)

# Add and configure target
add_executable(${TARGET} ${SOURCES})
target_include_directories(${TARGET} PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(${TARGET} 
  gtest
  gmock
  pthread 
  SpinTimer)

gtest_add_tests(TARGET ${TARGET})

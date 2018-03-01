
cmake_minimum_required(VERSION 3.5.1)

project(AEF VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/${CMAKE_BUILD_TYPE})

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX=1 -fPIC -MMD -MP -Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")

    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g")

endif()

# detect operating system
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-DSYSTEM_LINUX)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions(-DSYSTEM_DARWIN)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "AIX")
    add_definitions(-DSYSTEM_AIX)
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-DSYSTEM_WINDOWS)
endif()

# detect host processor
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

message(STATUS "Target Processor ${CMAKE_TARGET_PROCESSOR}")
message(STATUS "Compiler ID ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C Compiler ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler ${CMAKE_CXX_COMPILER}")
message(STATUS "CC Compiler ${CMAKE_CC_COMPILER}")
message(STATUS "CXX Flags ${CMAKE_CXX_FLAGS}")

option(BUILD_AEF "Builds AEF" ON)
option(BUILD_TEST "Builds TEST" ON)

if(BUILD_AEF)
  add_subdirectory( source )
endif()

if(BUILD_TEST)
  add_subdirectory( test )
endif()


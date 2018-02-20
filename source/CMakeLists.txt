
# stop if cmake version below 2.8
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# enable c, and c++ language
enable_language(C CXX)

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_TARGET_PROCESSOR}/bin/release)

# location of additional cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    )

# guard against in-source builds and bad build-type strings
#include(safeguards)

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

# example how to set c++ compiler flags for GNU
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -DLINUX=1 -O0 -g -Wall -fprofile-arcs -ftest-coverage -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused -L/home/yackey/arm-libs")
    
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -fPIC -MMD -MP")
    set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g3 -fPIC -MMD -MP")
    
#    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP")
#    set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP")
    
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory( AefLib )


#if(${CMAKE_TARGET_PROCESSOR} STREQUAL "ARM")
#SET(CMAKE_C_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
#SET(CMAKE_CXX_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-g++)
#SET(CMAKE_CC_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
#endif()

message(STATUS "source TP ${CMAKE_TARGET_PROCESSOR}")
message(STATUS "source SP ${CMAKE_SYSTEM_PROCESSOR}")

message(STATUS "source ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "source ${CMAKE_C_COMPILER}")
message(STATUS "source ${CMAKE_CXX_COMPILER}")
message(STATUS "source ${CMAKE_CC_COMPILER}")

message(STATUS "AEF ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "AEF ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "AEF ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}")
message(STATUS "AEF ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
message(STATUS "AEF ${BUILD_AEF}")
message(STATUS "AEF ${CMAKE_AEF_SOURCE_DIR}")


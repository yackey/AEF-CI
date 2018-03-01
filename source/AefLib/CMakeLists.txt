<<<<<<< HEAD
=======
# stop if cmake version below 2.8
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

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

#message(STATUS "AefLib target processor ${CMAKE_TARGET_PROCESSOR}")

#if(${CMAKE_TARGET_PROCESSOR} STREQUAL "ARM")
#SET(CMAKE_C_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
#SET(CMAKE_CXX_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-g++)
#SET(CMAKE_CC_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
#endif()

message(STATUS "AefLib ${CMAKE_TARGET_PROCESSOR}")
message(STATUS "AefLib ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "AefLib ${CMAKE_C_COMPILER}")
message(STATUS "AefLib ${CMAKE_CXX_COMPILER}")
message(STATUS "AefLib ${CMAKE_CC_COMPILER}")

message(STATUS "AefLib(flags) ${CMAKE_CXX_FLAGS}")

#add_subdirectory(Linux)
>>>>>>> cfa64b68f61646c3365e8e5b8a7b0e5d77157b9b

add_library(AEF_ARM SHARED
    aefMutexBase.cpp
    aefQueueTaskBase.cpp
    aefSerialBase.cpp
    aefTaskBase.cpp
    aefTaskMessageBase.cpp
    aefTaskSafeBase.cpp
    aefTcpServerBase.cpp
    aefTimerBase.cpp
    Linux/aefException.cpp
    Linux/aefMutex.cpp
    Linux/aefQueueTask.cpp
    Linux/aefSerial.cpp
    Linux/aefTask.cpp
    Linux/aefTaskSafe.cpp
    Linux/aefTcpServer.cpp
    Linux/aefTimer.cpp
)

target_link_libraries(
   AEF_ARM
   pthread
   rt
   )
   
set_target_properties(AEF_ARM PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(AEF_ARM PROPERTIES SOVERSION 1)

# location of header files
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/source/AefLib
    ${PROJECT_SOURCE_DIR}/source/AefLib/Linux
    )

# set cdash buildname
set(BUILDNAME
    "${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_Fortran_COMPILER_ID}-${cmake_build_type_tolower}"
    CACHE STRING
    "Name of build on the dashboard"
    )

# make install and cpack
#include(install_target)

#install(TARGETS AEF_ARM LIBRARY DESTINATION ${AVTEC_BASE}/filesystem/usr/lib)


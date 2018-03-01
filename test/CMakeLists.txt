
<<<<<<< HEAD
=======
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

#if(${CMAKE_TARGET_PROCESSOR} STREQUAL "ARM")
#SET(CMAKE_C_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
#SET(CMAKE_CXX_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-g++)
#SET(CMAKE_CC_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
#Sendif()

message(STATUS "AefTest ${CMAKE_TARGET_PROCESSOR}")
message(STATUS "AefTest ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "AefTest ${CMAKE_C_COMPILER}")
message(STATUS "AefTest ${CMAKE_CXX_COMPILER}")
message(STATUS "AefTest ${CMAKE_CC_COMPILER}")

message(STATUS "AefLib(flags) ${CMAKE_CXX_FLAGS}")

# build executable
#add_executable(hello.x src/hello.F90)

>>>>>>> cfa64b68f61646c3365e8e5b8a7b0e5d77157b9b
# location of header files
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/source/AefLib
    ${PROJECT_SOURCE_DIR}/source/AefLib/Linux
    ${PROJECT_SOURCE_DIR}/ThirdParty/googletest-master/googlemock/include
    ${PROJECT_SOURCE_DIR}/ThirdParty/googletest-master/googletest/include
    )

# unit test executable
add_executable(
    AefTest
    Main.cpp
    MutexTests.cpp
    TimerTests.cpp
    )

# link unit test executable against google test
target_link_libraries(
    AefTest
    AEF_ARM
    ${PROJECT_SOURCE_DIR}/ThirdParty/thirdparty-libs/${CMAKE_TARGET_PROCESSOR}/libgmock.a
    rt
    pthread
    gcov
    )

# set cdash buildname
set(BUILDNAME
    "${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_Fortran_COMPILER_ID}-${cmake_build_type_tolower}"
    CACHE STRING
    "Name of build on the dashboard"
    )

#install(TARGETS AefTest RUNTIME DESTINATION ${AVTEC_BASE}/filesystem/opt)


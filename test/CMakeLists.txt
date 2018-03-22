
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup(TARGETS)
#conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
conan_basic_setup(NO_OUTPUT_DIRS)

# location of header files
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/source/AefLib
    ${PROJECT_SOURCE_DIR}/source/AefLib/Linux
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/gtest
    ${PROJECT_SOURCE_DIR}/include/gmock
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
    ${CONAN_LIBS}
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


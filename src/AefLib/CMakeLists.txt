
add_library(AEF_ARM SHARED
    Linux/aefException.cpp
    Linux/aefMutex.cpp
    Linux/aefQueueTask.cpp
    Linux/aefSerial.cpp
    Linux/aefTask.cpp
    Linux/aefTaskSafe.cpp
    Linux/aefTcpServer.cpp
    Linux/aefTimer.cpp
    Linux/aefTaskMessage.cpp
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
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/Linux
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


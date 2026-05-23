# Build the executables

# Load simpleble library
include(FetchContent)
FetchContent_Declare(
                     simplecble
                     URL
                     https://github.com/simpleble/simpleble/releases/download/v0.12.1/libsimplecble_macos-aarch64.zip
                     
                     )
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(simplecble)
add_library(simplecble STATIC IMPORTED GLOBAL)

set_target_properties(simplecble PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${simplecble_SOURCE_DIR}/static/include")
set_target_properties(simplecble PROPERTIES IMPORTED_LOCATION "${simplecble_SOURCE_DIR}/static/lib/libsimplecble.a")

add_library(simpleble STATIC IMPORTED GLOBAL)
add_library(libxml2 STATIC IMPORTED GLOBAL)

set_target_properties(simpleble PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${simplecble_SOURCE_DIR}/static/include")
set_target_properties(simpleble PROPERTIES IMPORTED_LOCATION "${simplecble_SOURCE_DIR}/static/lib/libsimpleble.a")

get_property(MYDIR TARGET simplecble PROPERTY IMPORTED_LOCATION)
message(STATUS "DIR1 is " ${MYDIR})
get_property(MYDIR TARGET simpleble PROPERTY IMPORTED_LOCATION)
message(STATUS "DIR2 is " ${MYDIR})

# rdsdump
set(RDSDUMPSRC_FILES
    rdsdump.c
    print.c
    filters.c)
add_executable(rdsdump ${RDSDUMPSRC_FILES})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib "${simplecble_SOURCE_DIR}/static/include")
target_link_libraries(rdsdump simplecble simpleble rds  "-framework IOKit" "-framework IOBluetooth" "-framework CoreBluetooth"  "-framework Foundation" c++)

# rdstmc
find_package(sqlite3)
set(RDSTMCSRC_FILES
    rdstmc.c
    print.c
    filters.c
    traff.h
    traff.c
)
add_executable(rdstmc ${RDSTMCSRC_FILES})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib
target_link_libraries(rdstmc simplecble simpleble rds  "-framework IOKit" "-framework IOBluetooth" "-framework CoreBluetooth"  "-framework Foundation" c++ sqlite3)

# rdsrtcm
#set(RDSRTCMSRC_FILES
#rdstmc.c
#)
#add_executable(rdsrtcm ${RDSRTCMSRC_FILES})



cmake_minimum_required(VERSION 4.3.1)

project(radiodatasystem)
add_compile_definitions(ODA_TMC DATADIR="${CMAKE_CURRENT_SOURCE_DIR}/db" SHAREDSTATEDIR="${CMAKE_CURRENT_SOURCE_DIR}/db")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
find_package(SQLite3)
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(db)

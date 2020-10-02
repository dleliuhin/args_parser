#========================================================================================
cmake_minimum_required(VERSION 2.8)

project( test_args_parser )

set( VLIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../VLib" )

include( "${VLIBS_DIR}/cmake/gtest.cmake" )

include ( "../args_parser.cmake" )

add_executable( ${PROJECT_NAME} ${V_HEADERS} ${V_SOURCES} "main.cpp" )

target_link_libraries( ${PROJECT_NAME} ${V_LIBRARIES} )

#========================================================================================

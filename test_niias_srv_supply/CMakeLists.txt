#========================================================================================
cmake_minimum_required(VERSION 2.8)

project( test_niias_srv_supply )

set( VLIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../vlibs2" )

include( "${VLIBS_DIR}/cmake/gtest.cmake" )

include ( "../niias_srv_supply.cmake" )

add_executable( ${PROJECT_NAME} ${V_HEADERS} ${V_SOURCES} "main.cpp" )

target_link_libraries( ${PROJECT_NAME} ${V_LIBRARIES} )

#========================================================================================

#========================================================================================
if ( NOT  args_parser_INCLUDED )
    set ( args_parser_INCLUDED TRUE )

    message( "=== include args_parser... ===" )

    include( "${VLIBS_DIR}/cmake/vgit.cmake" )
    include( "${VLIBS_DIR}/cmake/vposix.cmake" )
    include( "${VLIBS_DIR}/cmake/vsettings.cmake" )
    include( "${VLIBS_DIR}/cmake/vbyte_buffer.cmake" )
    include( "${VLIBS_DIR}/cmake/vcmdline_parser.cmake" )

    include_directories( "${CMAKE_CURRENT_LIST_DIR}/" )

    set( V_HEADERS ${V_HEADERS} "${CMAKE_CURRENT_LIST_DIR}/arguments.h"   )
    set( V_SOURCES ${V_SOURCES} "${CMAKE_CURRENT_LIST_DIR}/arguments.cpp" )

    message( "=== args_parser included ===" )
endif()
# args_parser.cmake
#========================================================================================

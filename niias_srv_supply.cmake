#########################################################################################
##
##  VLIBS codebase, NIIAS
##
##  GNU Lesser General Public License Usage
##  This file may be used under the terms of the GNU Lesser General Public License
##  version 3 as published by the Free Software Foundation and appearing in the file
##  LICENSE.LGPL3 included in the packaging of this file. Please review the following
##  information to ensure the GNU Lesser General Public License version 3 requirements
##  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
#########################################################################################
# niias_srv_supply.cmake

#========================================================================================
if ( NOT  niias_srv_supply_INCLUDED )
    set ( niias_srv_supply_INCLUDED TRUE )

    message( "=== include niias_srv_supply... ===" )

    include( "${VLIBS_DIR}/cmake/vgit.cmake" )
    include( "${VLIBS_DIR}/cmake/vposix.cmake" )
    include( "${VLIBS_DIR}/cmake/vsettings.cmake" )
    include( "${VLIBS_DIR}/cmake/vbyte_buffer.cmake" )
    include( "${VLIBS_DIR}/cmake/vcmdline_parser.cmake" )

    include_directories( "${CMAKE_CURRENT_LIST_DIR}/" )

    set( V_HEADERS ${V_HEADERS} "${CMAKE_CURRENT_LIST_DIR}/niias_arguments.h"   )
    set( V_SOURCES ${V_SOURCES} "${CMAKE_CURRENT_LIST_DIR}/niias_arguments.cpp" )

    message( "=== niias_srv_supply included ===" )
endif()
# niias_srv_supply.cmake
#========================================================================================

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
# niias_srv_supply.pri

#========================================================================================
isEmpty(qi_niias_srv_supply) {
    qi_niias_srv_supply = 1;
    isEmpty(qi_not_print_pri_messages): message(">>> niias_srv_supply appended <<<")

    isEmpty(VLIBS_DIR): error("niias_srv_supply: Need VLIBS_DIR correct path.")

    include( $$VLIBS_DIR/qmake/vgit.pri )
    include( $$VLIBS_DIR/qmake/vposix.pri )
    include( $$VLIBS_DIR/qmake/vsettings.pri )
    include( $$VLIBS_DIR/qmake/vbyte_buffer.pri )
    include( $$VLIBS_DIR/qmake/vcmdline_parser.pri )

    lessThan(VLIBS_REVCOUNT, 198): error( "VLIBS is too young :(" )

    INCLUDEPATH += $$PWD

    HEADERS     += $$PWD/niias_arguments.h
    SOURCES     += $$PWD/niias_arguments.cpp

    OTHER_FILES += $$PWD/niias_srv_supply.cmake
}
# niias_srv_supply.pri
#========================================================================================


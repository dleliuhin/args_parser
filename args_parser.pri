#========================================================================================
isEmpty(qi_args_parser) {
    qi_args_parser = 1;
    isEmpty(qi_not_print_pri_messages): message(">>> args_parser appended <<<")

    isEmpty(VLIBS_DIR): error("args_parser: Need VLIBS_DIR correct path.")

    include( $$VLIBS_DIR/qmake/vgit.pri )
    include( $$VLIBS_DIR/qmake/vposix.pri )
    include( $$VLIBS_DIR/qmake/vsettings.pri )
    include( $$VLIBS_DIR/qmake/vbyte_buffer.pri )
    include( $$VLIBS_DIR/qmake/vcmdline_parser.pri )

    lessThan(VLIBS_REVCOUNT, 198): error( "VLIBS is too young :(" )

    INCLUDEPATH += $$PWD

    HEADERS     += $$PWD/arguments.h
    SOURCES     += $$PWD/arguments.cpp

    OTHER_FILES += $$PWD/args_parser.cmake
    OTHER_FILES += $$PWD/README.md
}
# args_parser.pri
#========================================================================================


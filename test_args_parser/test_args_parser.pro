TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

CONFIG -= console
#CONFIG -= qt  # test with qt.

VLIBS_DIR = $$PWD/../../vlibs2
include( $$VLIBS_DIR/qmake/gtest.pri )
include( $$VLIBS_DIR/qmake/vlog.pri )

MAIN_DIR = $$PWD
include( $$PWD/../args_parser.pri )

SOURCES += main.cpp

DEFINES += V_DEVELOP

EXTERNAL_BASE_PATH = ../../
include($$EXTERNAL_BASE_PATH/ext_common.pri)
TEMPLATE = lib
DEFINES += _UNICODE
TARGET = muparser
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += ../include
macx:QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-format -Wno-pointer-sign -Wno-deprecated-declarations\
  -Wno-deprecated-register -Wno-delete-non-virtual-dtor -Wno-logical-not-parentheses -Wno-switch -Wno-unknown-pragmas
macx:QMAKE_CXXFLAGS_WARN_ON = 


# Input
SOURCES += \
    muParser.cpp \
    muParserBase.cpp \
    muParserBytecode.cpp \
    muParserCallback.cpp \
    muParserDLL.cpp \
    muParserError.cpp \
    muParserInt.cpp \
    muParserTest.cpp \
    muParserTokenReader.cpp

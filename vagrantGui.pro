QT       += core gui script

TARGET = vagrantGui
TEMPLATE = app

win32 {
    LIBS += -lOle32 -lOleAut32
    SOURCES += sdk/bindings/mscom/lib/VirtualBox_i.c
    INCLUDEPATH += sdk/bindings/mscom/include
}

unix {
    HEADERS += sdk/bindings/xpcom/cbinding/VBoxXPCOMCGlue.h
    SOURCES += sdk/bindings/xpcom/cbinding/VBoxXPCOMCGlue.c
    INCLUDEPATH += sdk/bindings/xpcom/include\
                sdk/bindings/xpcom/include/xpcom\
                sdk/bindings/xpcom/include/string\
                sdk/bindings/xpcom/include/nsprpub

    macx {
        INCLUDEPATH += /Applications/Xcode.app/Contents/Developer/Headers/FlatCarbon

        DEFINES += _M_AMD64\
                    RT_OS_DARWIN

        QMAKE_LFLAGS += -framework Carbon
    } else {
        DEFINES += RT_OS_LINUX
    }
}


HEADERS  += mainwindow.h \
        virtualboxunifiedinterface.h \
        vagrantinterface.h

SOURCES += main.cpp\
        mainwindow.cpp \
        virtualboxunifiedinterface.cpp \
        vagrantinterface.cpp

RESOURCES += data.qrc


include(toneanalyzer.pri)

#-------------------------------------------------
#
# Project created by QtCreator 2019-02-11T14:45:29
#
#-------------------------------------------------

QT       += core gui multimedia widgets uiplugin charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ToneAnalyzer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    levelmeter.cpp \
    frequencyspectrum.cpp \
    audioanalyzerthread.cpp \
    audioengine.cpp \
    audioinputthread.cpp \
    util.cpp

HEADERS += \
        mainwindow.h \
    levelmeter.h \
    frequencyspectrum.h \
    audioanalyzerthread.h \
    audioengine.h \
    audioinputthread.h \
    util.h \
    notes.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    toneanalyzer.pri

# Libraries for fftw3 32 bits does not seem to be working... Must compile with 64 bits for now.
#!contains(QMAKE_TARGET.arch, x86_64) {
#    message( "Building for 32 bits machine...")
#    LIBS += -L$$PWD/vendor/fftw/fftw-3.3.5-dll32 -lfftw3-3 -lfftw3l-3 -lfftw3f-3

#    INCLUDEPATH += $$PWD/vendor/fftw/fftw-3.3.5-dll32
#    DEPENDPATH += $$PWD/vendor/fftw/fftw-3.3.5-dll32
#}
#else {
    message( "Building for 64 bits machine...")
    LIBS += -L$$PWD/vendor/fftw/fftw-3.3.5-dll64 -lfftw3-3 -lfftw3l-3 -lfftw3f-3

    INCLUDEPATH += $$PWD/vendor/fftw/fftw-3.3.5-dll64
    DEPENDPATH += $$PWD/vendor/fftw/fftw-3.3.5-dll64
#}


INCLUDEPATH += C:\Dev\boost_1_69_0
DEPENDPATH += C:\Dev\boost_1_69_0

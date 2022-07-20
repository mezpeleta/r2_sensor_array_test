QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    sensorarraytest.cpp

HEADERS += \
    defines.h \
    sensorarraytest.h

FORMS += \
    sensorarraytest.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/Basler/include
LIBS += "-L$$PWD/Basler/lib/x64"

#INCLUDEPATH += $$PWD/libusb/include
#DEPENDPATH += $$PWD/libusb/VS2015-x64

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Intel/lib/x64/ -lrealsense2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Intel/lib/x64/ -lrealsense2d
else:unix: LIBS += -L$$PWD/Intel/lib/x64/ -lrealsense2

INCLUDEPATH += $$PWD/Intel/lib/x64
DEPENDPATH += $$PWD/Intel/lib/x64

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libusb/VS2015-x64/lib/ -llibusb-1.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libusb/VS2015-x64/lib/ -llibusb-1.0d
else:unix: LIBS += -L$$PWD/libusb/VS2015-x64/lib/ -llibusb-1.0

INCLUDEPATH += $$PWD/libusb/VS2015-x64
DEPENDPATH += $$PWD/libusb/VS2015-x64

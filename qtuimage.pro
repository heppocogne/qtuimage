QT       += core gui svg concurrent

VERSION = 0.0.1.1

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    configure.cpp \
    image_loader.cpp \
    image_viewer.cpp \
    image_xform.cpp \
    main.cpp \
    mainwindow.cpp \
    thumbnail_loader.cpp \
    thumbnails_container.cpp \
    toolbar.cpp

HEADERS += \
    configure.h \
    image_loader.h \
    image_viewer.h \
    image_xform.h \
    mainwindow.h \
    thumbnail_loader.h \
    thumbnails_container.h \
    toolbar.h

DEPLOY_COMMAND = windeployqt

RC_ICONS = qtuimage_icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

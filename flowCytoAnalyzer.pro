QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    custompointselection.cpp \
    datasetcontainer.cpp \
    information.cpp \
    main.cpp \
    mainwindow.cpp \
    parser.cpp \
    parsercsv.cpp \
    parserfcs.cpp \
    plotwindow.cpp \
    qcustomplot.cpp \
    settings.cpp

HEADERS += \
    custompointselection.h \
    datasetcontainer.h \
    information.h \
    mainwindow.h \
    parser.h \
    parsercsv.h \
    parserfcs.h \
    plotwindow.h \
    qcustomplot.h \
    settings.h

FORMS += \
    information.ui \
    mainwindow.ui \
    settings.ui

INCLUDEPATH += D:/qt_code/eigen

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

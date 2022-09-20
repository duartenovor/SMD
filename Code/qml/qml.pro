QT += quick widgets

CONFIG += c++11

CONFIG += console qml_debug

LIBS += \
        -pthread -lpthread -lrt -lsqlite3

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/CCaregiver.cpp \
        src/CDatabase.cpp \
        src/CMedicine.cpp \
        src/CPatient.cpp \
        src/CPatientNotification.cpp \
        src/CPrescription.cpp \
        src/CTime.cpp \
        src/auxiliaries.cpp \
        src/smd.cpp

HEADERS += \
        inc/CCaregiver.h \
        inc/CDatabase.h \
        inc/CMedicine.h \
        inc/CPatient.h \
        inc/CPatientNotification.h \
        inc/CPrescription.h \
        inc/CTime.h \
        inc/auxiliaries.h \
        inc/smd.h

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /etc/SMD
!isEmpty(target.path): INSTALLS += target

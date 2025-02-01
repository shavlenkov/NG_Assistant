QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/assistant.cpp \
    src/aiworker.cpp \
    src/controlpanel.cpp \
    src/util.cpp

HEADERS += \
    include/assistant.h \
    include/aiworker.h \
    include/controlpanel.h \
    include/util.h \
    include/config.h

FORMS += \
    ui/assistant.ui \
    ui/controlpanel.ui

RESOURCES += \
    resources/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

exists($$PWD/.env) {
    !exists($$OUT_PWD/.env) {
        QMAKE_POST_LINK += cp \"$$PWD/.env\" \"$$OUT_PWD/\"
    }
}

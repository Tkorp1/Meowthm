TARGET = Meowthm

QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gamescene.cpp \
    ButtonItem.cpp \
    main.cpp \
    mainwindow.cpp \
    mapparser.cpp \
    note.cpp \
    track.cpp

HEADERS += \
    gamescene.h \
    AchievementsWindow.h \
    ButtonItem.h \
    PokeWindow.h \
    SelectSongWindow.h \
    SettingsWindow.h \
    mainwindow.h \
    mapparser.h \
    note.h \
    track.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

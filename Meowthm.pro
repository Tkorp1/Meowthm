TARGET = Meowthm

QT += core gui widgets multimedia multimediawidgets
QT += widgets multimedia # 音频

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SelectSongWindow.cpp \
    analysiswindow.cpp \
    gameconfig.cpp \
    gamescene.cpp \
    ProfileWindow.cpp \
    gamestate.cpp \
    main.cpp \
    mainwindow.cpp \
    mapparser.cpp \
    note.cpp \
    resultscene.cpp \
    track.cpp \
    SettingsWindow.cpp \
    AchievementsWindow.cpp \
    PokeWindow.cpp


HEADERS += \
    SelectSongWindow.h \
    analysiswindow.h \
    gameconfig.h \
    gamescene.h \
    AchievementsWindow.h \
    PokeWindow.h \
    ProfileWindow.h \
    SettingsWindow.h \
    gamestate.h \
    mainwindow.h \
    mapparser.h \
    note.h \
    resultscene.h \
    track.h \

FORMS += \
    analysiswindow.ui \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
QMAKE_POST_LINK += $$quote(cp -r $$PWD/maps $$OUT_PWD/)

RESOURCES += \
    resources.qrc

DISTFILES += \
    poke1.png \
    poke2.png \
    poke3.png \
    poke_bg.png

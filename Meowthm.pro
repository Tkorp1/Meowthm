TARGET = Meowthm

QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gamescene.cpp \       # 注释掉
    ButtonItem.cpp \
    ProfileWindow.cpp \
    main.cpp \
    mainwindow.cpp \
    mapparser.cpp \       # 注释掉
    note.cpp \            # 注释掉
    track.cpp \           # 注释掉
    SelectSongWindow.cpp \
    SettingsWindow.cpp \
    AchievementsWindow.cpp \
    PokeWindow.cpp


HEADERS += \
    gamescene.h \         # 注释掉
    AchievementsWindow.h \
    ButtonItem.h \
    PokeWindow.h \
    ProfileWindow.h \
    SelectSongWindow.h \
    SettingsWindow.h \
    mainwindow.h \
    mapparser.h \         # 注释掉
    note.h \              # 注释掉
    track.h \             # 注释掉

FORMS += \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
QMAKE_POST_LINK += $$quote(cp -r $$PWD/maps $$OUT_PWD/)

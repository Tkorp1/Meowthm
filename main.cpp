#include "mainwindow.h"
#include "gamescene.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 为了试验gamescene先注释掉
    // MainWindow w;
    // w.show();
    GameScene* testGame = new GameScene("/Users/rose/code/QT/Meowthm/maps/song1"); // 随便传个假路径，反正我们在 GameScene 里已经造了假音符
    testGame->show();

    return a.exec();

}

#include "mainwindow.h"
#include "gamescene.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 为了试验gamescene先注释掉
    // MainWindow w;
    // w.show();

    GameScene* testGame = new GameScene(":/map/maps/song1"); // 现在在选曲界面还没写的时候，先搞成这个，等选曲界面写完了接
    testGame->show();

    return a.exec();

}

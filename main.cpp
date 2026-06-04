#include "mainwindow.h"
#include "gamescene.h"
#include "resultscene.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 为了试验gamescene先注释掉
    // MainWindow w;
    // w.show();

    GameScene* testGame = new GameScene("/Users/tkorp/Documents/GitHub/Meowthm1/maps/song1"); // 这里写的是绝对地址，到时候记得改
    testGame->show();
    //GameState temp("song");
    //ResultScene* testresult = new ResultScene(temp);
    //testresult->show();
    return a.exec();

}

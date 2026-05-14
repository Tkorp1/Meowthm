#include "gamescene.h"
#include "mapparser.h"

GameScene::GameScene(QWidget *parent)
    : QWidget{parent}
{
    int __hitLineY = 100;
    int trackWidth = 200;
    int xpos = 300;
    for(int jj = 0; jj < 4;++jj){
        Track[jj] = new Track(jj, __hitLineY, jj * trackWidth + xpos, this);
        //下面开始为轨道使用mapparser创建音符
        MapParser temp=MapParser();
        temp(&(Track[jj]));
    }
}

#include "gamescene.h"
#include "mapparser.h"

GameScene::GameScene(QWidget *parent)
    : QWidget{parent}
{
    // 0.设置gamescene的基本形状
    // 大小
    this->setFixedSize(800, 600);
    // 颜色
    this->setStyleSheet("background-color: #2b2b2b;"); // 给个深色护眼背景


    // 1.初始化参数
    globalSpeed = 0.5;
    int _hitLineY = 100;

    // 2.创建轨道
    int trackWidth = 20;
    int leftX = 300;


    MapParser mp = MapParser(this);
    for(int i = 0; i < 4; i++){
        tracks[i] = new Track(i, _hitLineY, leftX + trackWidth * i, this);
        mp(&tracks[i]);
    }

    //3.建立闹钟
    updateTimer = new QTimer(this);
    // 连接信号槽：每次闹钟响就执行gameLoop
    connect(updateTimer, &QTimer::timeout, this, &GameScene::gameLoop);
    //设定闹钟间隔 目前16s
    updateTimer->start(16);
}

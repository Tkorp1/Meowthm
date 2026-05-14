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
        connect(tracks[i], &Track::noteJudged, this, &GameScene::hitNoteJudge);
        mp(&tracks[i]);
    }

    //3.建立闹钟
    updateTimer = new QTimer(this);
    // 连接信号槽：每次闹钟响就执行gameLoop
    connect(updateTimer, &QTimer::timeout, this, &GameScene::gameLoop);
    //设定闹钟间隔 目前16s
    updateTimer->start(16);
}

GameScene::~GameScene(){

}

void GameScene::keyPressEvent(QKeyEvent *event){
    // D F J K
    if(event -> key() == Qt::Key_D){
        tracks[0] -> checkHit(currentMusicTime);
    }else if(event -> key() == Qt::Key_F){
        tracks[1] -> checkHit(currentMusicTime);
    }else if(event -> key() == Qt::Key_J){
        tracks[2] -> checkHit(currentMusicTime);
    }else if(event -> key() == Qt::Key_K){
        tracks[3] -> checkHit(currentMusicTime);
    }
}

void GameScene::keyReleaseEvent(QKeyEvent * event){
    // 留给以后的自己
}


// 主循环函数
void GameScene::gameLoop(){
    // 以后写了音乐播放器，再实时播报时间，现在先简单模拟一下递增
    currentMusicTime += 16;

    // 更新轨道
    for (int i = 0; i < 4; ++i) {
        tracks[i]->updateTrack(currentMusicTime, globalSpeed);
    }

}



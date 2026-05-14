#include "gamescene.h"
#include "mapparser.h"


#include <QPainter>

GameScene::GameScene(QString _mapPath, QWidget *parent)
    : QWidget{parent},
    mapPath(_mapPath),
    currentMusicTime(0), //初始化为 0！！！
    currentScore(0),
    currentCombo(0)
{
    // 0.设置gamescene的基本形状
    // 大小
    this->setFixedSize(800, 600);
    // 颜色
    this->setStyleSheet("background-color: #2b2b2b;");

    // 0.5补充
    // 强制获取键盘焦点，否则接收不到按键(((
    // 气笑了
    this->setFocusPolicy(Qt::StrongFocus);

    // 1.初始化参数
    globalSpeed = 0.5;
    int _hitLineY = 500;

    // 2.与ui相关的部件
    scoreLabel = new QLabel(this);
    scoreLabel -> setText("Score: 0");
    scoreLabel -> setGeometry(600, 20, 180, 50);
    scoreLabel -> setStyleSheet("color: #FFD700; font-size: 28px; font-weight: bold; background-color: transparent;");
    scoreLabel ->show();

    comboLabel = new QLabel(this);
    comboLabel -> setText("Combo: 0");
    comboLabel -> setGeometry(350, 200, 150, 50);
    comboLabel -> setStyleSheet("color: #00BFFF; font-size: 36px; font-weight: bold; background-color: transparent;");
    comboLabel -> show();

    // 3.创建轨道
    int trackWidth = 100;
    int leftX = 200;


    // 这里等写完 mapparser以后再改一下
    MapParser mp = MapParser(mapPath ,this);
    for(int i = 0; i < 4; i++){
        tracks[i] = new Track(i, _hitLineY, leftX + trackWidth * i, this);
        connect(tracks[i], &Track::noteJudged, this, &GameScene::hitNoteJudge);
        mp(&tracks[i]);
    }


    //4.建立闹钟
    updateTimer = new QTimer(this);
    // 连接信号槽：每次闹钟响就执行gameLoop
    connect(updateTimer, &QTimer::timeout, this, &GameScene::gameLoop);
    //设定闹钟间隔 目前16s
    updateTimer->start(16);
}

GameScene::~GameScene(){

}

void GameScene::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 0.轨道
    painter.setPen(QPen(Qt::white, 2));
    for(int i = 0; i <= 4; ++i){
        int x = 200 + i * 100;
        painter.drawLine(x, 0, x, this -> height());

    }

    // 1.判定线
    painter.setPen(QPen(QColor(255, 215, 0), 4));
    painter.drawLine(200, hitLineY, 600, hitLineY);

}


QString GameScene::getMapPath() const{
    return mapPath;
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


void GameScene::hitNoteJudge(int result){
    if(result == 1){ // Miss
        // 连击中断
        currentCombo = 0;
        //comboLabel->show();
    }else if(result >= 2){
        currentCombo++;
        // 目前暂定给分，之后调整
        currentScore += (result == 3) ? 100 : 70;
        comboLabel -> setText(QString("Combo: %1").arg(currentCombo));
        //comboLabel->show();
    }
    scoreLabel->setText(QString("Score: %1").arg(currentScore));

    // 之后的特效在这里写
}




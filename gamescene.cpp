#include "gamescene.h"
#include "mapparser.h"
#include "gameconfig.h"
#include "resultscene.h"

#include <QPainter>
#include <QDir>
#include <QSoundEffect>

GameScene::GameScene(QString _mapPath, QWidget *parent)
    : QWidget{parent},
    mapPath(_mapPath),
    currentMusicTime(0), //初始化为 0！！！
    state(GameState()),
    gameEnded(false)
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

    // 注意：保证窗口“关闭即销毁”，防止内存泄漏
    this->setAttribute(Qt::WA_DeleteOnClose);

    // 1.初始化参数

    // 先读取当前配置
    globalSpeed = GameConfig::instance()->getNoteSpeed();
    offset = GameConfig::instance()->getCurrentOffset();

    // 监听流速变化
    connect(GameConfig::instance(),
            &GameConfig::noteSpeedChanged,
            this,
            [this](double newSpeed){
                globalSpeed = newSpeed;
            });

    hitLineY = 500;

    // 2.与ui相关的部件
    scoreLabel = new QLabel(this);
    scoreLabel -> setText("Score: 0");
    scoreLabel -> setGeometry(600, 20, 180, 50);
    scoreLabel -> setStyleSheet("color: #FFD700; font-size: 28px; font-weight: bold; background-color: transparent;");
    scoreLabel ->show();

    comboLabel = new QLabel(this);
    comboLabel -> setText("0");
    comboLabel -> setGeometry(400, 200, 150, 50);
    comboLabel -> setStyleSheet("color: #00BFFF; font-size: 36px; font-weight: bold; background-color: transparent;");
    comboLabel -> show();

    accuracyLabel = new QLabel(this);
    accuracyLabel -> setText("Acc: 100.00%");
    accuracyLabel -> setGeometry(600, 70, 180, 50);
    accuracyLabel -> setStyleSheet("color: #98FB98; font-size: 24px; font-weight: bold; background-color: transparent;");
    accuracyLabel -> show();

    // 3.创建轨道
    int trackWidth = 100;
    int leftX = 200;


    QDir dir(mapPath);

    MapParser mp = MapParser(mapPath);
    for(int i = 0; i < 4; i++){
        tracks[i] = new Track(i, hitLineY, leftX + trackWidth * i, this);
        connect(tracks[i], &Track::noteJudged, this, &GameScene::hitNoteJudge);
        //文件操作，获取轨道的路径
        QString fileName = QString("Track%1.txt").arg(i);

        QString path = dir.filePath(fileName);

        //获取list
        QList<Note*> noteListTemp=mp.parse(path);
        tracks[i]->addNotes(noteListTemp);
        tracks[i]->setNoteParent(this);
    }

    allMusicTime = mp.getMusicTime();

    // 4.音乐引擎
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    QString musicPath = dir.filePath("music.mp3");
    player->setSource(QUrl::fromLocalFile(musicPath));

    connect(player,
            &QMediaPlayer::mediaStatusChanged,
            this,
            [this](QMediaPlayer::MediaStatus status){
                if(status ==
                    QMediaPlayer::EndOfMedia){
                    gameOver();
                }
            });

    audioOutput->setVolume(0.5);

    player->play();

    // 4.5设置打击音效引擎
    hitSound = new QSoundEffect(this);
    // 先放绝对地址
    hitSound->setSource(QUrl::fromLocalFile("/Users/rose/sound_effect/dong.wav"));
    // 声音大小
    hitSound->setVolume(0.65f);

    // 5.建立闹钟
    updateTimer = new QTimer(this);
    // 连接信号槽：每次闹钟响就执行gameLoop
    connect(updateTimer, &QTimer::timeout, this, &GameScene::gameLoop);
    // 设定闹钟间隔 目前16s
    updateTimer->start(16);


}

GameScene::~GameScene(){

}

void GameScene::gameOver(){
    if(gameEnded){
        return;
    }
    gameEnded = true;

    updateTimer->stop();

    player->stop();

    ResultScene* resultscene = new ResultScene(state);

    resultscene->show();

    close();

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
        hitSound->play();
    }else if(event -> key() == Qt::Key_F){
        tracks[1] -> checkHit(currentMusicTime);
        hitSound->play();
    }else if(event -> key() == Qt::Key_J){
        tracks[2] -> checkHit(currentMusicTime);
        hitSound->play();
    }else if(event -> key() == Qt::Key_K){
        tracks[3] -> checkHit(currentMusicTime);
        hitSound->play();
    }
}

void GameScene::keyReleaseEvent(QKeyEvent * event){
    // 留给以后的自己
}


// 主循环函数
void GameScene::gameLoop(){
    // 实时播报时间
    currentMusicTime = player->position();

    // 更新轨道
    for (int i = 0; i < 4; ++i) {
        tracks[i]->updateTrack(currentMusicTime - offset, globalSpeed);
    }

    // 如果游戏结束了
    if(currentMusicTime >= allMusicTime + 2000){
        gameOver();
    }

}


void GameScene::hitNoteJudge(int result){
    // 首先修改所有的状态
    state.changeCurrentState(result);
    // 然后更改显示
    comboLabel->setText(QString("%1").arg(state.getCurrentCombo()));

    scoreLabel->setText(QString("Score: %1").arg(state.getCurrentScore()));

    accuracyLabel->setText(QString("Acc: %1%").arg(state.getCurrentAcc() * 100, 0, 'f', 2));


}




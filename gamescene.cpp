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
    gameEnded(false),
    isPaused(false)
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
        // 同时增加两秒的时间
        tracks[i]->setNoteParent(this);
    }

    allMusicTime = mp.getMusicTime();

    // 4.音乐引擎
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    QString musicPath = dir.filePath("music.mp3");

    // 判断这是内置歌曲，还是外部自定义歌曲
    if (musicPath.startsWith(":")) {
        // 如果路径是以 ":" 开头，说明它来自.qrc
        player->setSource(QUrl("qrc" + musicPath));
    } else {
        // 否则，说明它是电脑硬盘上的外部歌曲
        player->setSource(QUrl::fromLocalFile(musicPath));
    }

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
    hitSound->setSource(QUrl("qrc:/sound/sounds/dong.wav"));
    // 声音大小
    hitSound->setVolume(0.65f);



    // 5.建立闹钟
    updateTimer = new QTimer(this);
    // 连接信号槽：每次闹钟响就执行gameLoop
    connect(updateTimer, &QTimer::timeout, this, &GameScene::gameLoop);
    // 设定闹钟间隔 目前16s
    updateTimer->start(16);

    // 5.5 创建顶层轨道线与判定线
    // 画 5 条垂直轨道分割线
    for(int i = 0; i <= 4; ++i){
        int x = 200 + i * 100;
        QFrame* vLine = new QFrame(this);
        vLine->setGeometry(x - 1, 0, 2, this->height()); // 宽度为2
        // 使用半透明白色
        vLine->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
        vLine->setAttribute(Qt::WA_TransparentForMouseEvents); // 鼠标穿透，防挡底层事件

        // 【核心魔法】：把这条线强行拉到最顶层，死死压在音符上面！
        vLine->raise();
    }

    // 画 1 条水平判定线
    QFrame* hitLineUI = new QFrame(this);
    hitLineUI->setGeometry(200, hitLineY - 2, 400, 4); // 宽度为4
    hitLineUI->setStyleSheet("background-color: rgba(255, 215, 0, 200);"); // 半透明金色
    hitLineUI->setAttribute(Qt::WA_TransparentForMouseEvents);
    hitLineUI->raise(); // 同样拉到最顶层！

    // 创建轨道发光特效
    for (int i = 0; i < 4; ++i) {
        trackHighlights[i] = new QFrame(this);
        // 设定位置：和轨道一样宽，高度从屏幕顶部一直到判定线
        trackHighlights[i]->setGeometry(200 + i * 100, 0, 100, hitLineY);

        // 使用 CSS 线性渐变！从底部的半透明白色，向上渐变成完全透明
        trackHighlights[i]->setStyleSheet(
            "background: qlineargradient(x1:0, y1:1, x2:0, y2:0, "
            "stop:0 rgba(255, 255, 255, 120), stop:1 rgba(255, 255, 255, 0));"
            );
        trackHighlights[i]->setAttribute(Qt::WA_TransparentForMouseEvents);

        // 默认隐藏，不按不亮
        trackHighlights[i]->hide();

        // 让它稍微靠下一点，不要盖住掉落的音符
        trackHighlights[i]->lower();
    }


    initPauseUI();

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

}


QString GameScene::getMapPath() const{
    return mapPath;
}


void GameScene::keyPressEvent(QKeyEvent *event){
    // 长按触发的自动重复，忽略x
    if (event->isAutoRepeat()) {
        return;
    }

    // esc
    if (event->key() == Qt::Key_Escape) {
        if (!isPaused) {
            pauseGame(); // 如果没暂停，就暂停
        } else {
            // 如果已经暂停了
            if (countdownTimer->isActive()) {
                // 如果正在倒计时，按esc就打断倒计时，重新暂停
                countdownTimer->stop();
                countdownLabel->hide();
                btnContinue->show();
                btnRestart->show();
                btnQuit->show();
            } else {
                // 如果没在倒计时：按esc相当于点击“继续”
                btnContinue->click();
            }
        }
        return;
    }

    // 如果当前是暂停状态，任何按键都不生效，只有esc表示继续游戏
    if (isPaused) {
        return;
    }

    // D F J K
    if(event -> key() == Qt::Key_D){
        tracks[0] -> checkHit(currentMusicTime);
        hitSound->play();
        trackHighlights[0]->show();
    }else if(event -> key() == Qt::Key_F){
        tracks[1] -> checkHit(currentMusicTime);
        hitSound->play();
        trackHighlights[1]->show();
    }else if(event -> key() == Qt::Key_J){
        tracks[2] -> checkHit(currentMusicTime);
        hitSound->play();
        trackHighlights[2]->show();
    }else if(event -> key() == Qt::Key_K){
        tracks[3] -> checkHit(currentMusicTime);
        hitSound->play();
        trackHighlights[3]->show();
    }
}

void GameScene::keyReleaseEvent(QKeyEvent * event){
    // 长按触发的自动重复
    if (event->isAutoRepeat()) {
        return;
    }
    // 传给对应的轨道
    if(event -> key() == Qt::Key_D){
        tracks[0] -> isReleased(currentMusicTime);
        trackHighlights[0]->hide();
    }else if(event -> key() == Qt::Key_F){
        tracks[1] -> isReleased(currentMusicTime);
        trackHighlights[1]->hide();
    }else if(event -> key() == Qt::Key_J){
        tracks[2] -> isReleased(currentMusicTime);
        trackHighlights[2]->hide();
    }else if(event -> key() == Qt::Key_K){
        tracks[3] -> isReleased(currentMusicTime);
        trackHighlights[3]->hide();
    }
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


void GameScene::pauseGame() {
    if (gameEnded) return; // 如果游戏已经结束，就不允许暂停了

    isPaused = true;
    updateTimer->stop();   // 暂停画面
    player->pause();       // 暂停音乐

    // 重置暂停界面的状态，确保按钮全都在，倒计时藏起来
    btnContinue->show();
    btnRestart->show();
    btnQuit->show();
    if(countdownLabel) countdownLabel->hide();

    // 显示黑幕，强制提升到最顶层，盖住分数和掉落的音符
    pauseWidget->raise();
    pauseWidget->show();
}

void GameScene::resumeGame() {
    if (gameEnded) return;

    isPaused = false;
    updateTimer->start(16); // 恢复画面
    player->play();         // 恢复音乐

    // 隐藏黑幕
    pauseWidget->hide();
}


void GameScene::initPauseUI() {
    // 1. 创建黑背景
    pauseWidget = new QWidget(this);
    pauseWidget->setGeometry(0, 0, 800, 600);

    // 给面板起个专有名字且只对这个名字设置半透明，防止按钮被干扰！！！
    pauseWidget->setObjectName("pauseMask");
    pauseWidget->setStyleSheet("QWidget#pauseMask { background-color: rgba(0, 0, 0, 180); }");

    // 2. 创建三个图标（横排）
    int btnSize = 60;
    int gap = 70; // 按钮间距
    int totalWidth = (btnSize * 3) + (gap * 2); // 总宽度

    // 计算第一个按钮的X坐标，让整个按钮组绝对居中
    int startX = (800 - totalWidth) / 2;
    // 设定按钮在屏幕上的Y坐标）
    int btnY = 250;

    // 退出按钮 (左)
    btnQuit = new QPushButton("", pauseWidget);
    btnQuit->setGeometry(startX, btnY, btnSize, btnSize);
    btnQuit->setStyleSheet("QPushButton {"
                           "border-image: url(:/image/images/exit.png);"
                           "background-color: transparent;" // 背景透明
                           "}"
                           "QPushButton:hover {"
                           "background-color: rgba(255, 255, 255, 50);" //悬停时发亮
                           "border-radius: 15px;"
                           "}");

    // 继续按钮 (中)
    btnContinue = new QPushButton("", pauseWidget);
    btnContinue->setGeometry(startX + btnSize + gap, btnY, btnSize, btnSize);
    btnContinue->setStyleSheet("QPushButton {"
                               "border-image: url(:/image/images/continue.png);"
                               "background-color: transparent;"
                               "}"
                               "QPushButton:hover {"
                               "background-color: rgba(255, 255, 255, 50);"
                               "border-radius: 15px;"
                               "}");

    // 重来按钮 (右)
    btnRestart = new QPushButton("", pauseWidget);
    btnRestart->setGeometry(startX + (btnSize + gap) * 2, btnY, btnSize, btnSize);
    btnRestart->setStyleSheet("QPushButton {"
                              "border-image: url(:/image/images/restart.png);"
                              "background-color: transparent;"
                              "}"
                              "QPushButton:hover {"
                              "background-color: rgba(255, 255, 255, 50);"
                              "border-radius: 15px;"
                              "}");

    // 3. 默认隐藏，等按 ESC 再出来
    pauseWidget->hide();


    // 3.5 倒计时数字标签和闹钟
    countdownLabel = new QLabel(pauseWidget); // 放在黑屏上
    countdownLabel->setGeometry(0, 0, 800, 600);
    countdownLabel->setAlignment(Qt::AlignCenter); // 居中
    countdownLabel->setStyleSheet("color: white; font-size: 150px; font-weight: bold; background-color: transparent;");
    countdownLabel->hide(); // 隐藏

    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, [this]() {
        countdownValue--; // 每次“响铃”，数字减 1
        if (countdownValue > 0) {
            countdownLabel->setText(QString::number(countdownValue));
        } else {
            // 倒计时结束
            countdownTimer->stop();
            resumeGame(); // 真正恢复游戏此时才执行！！！
        }
    });


    // 4. 绑定按钮的“三岔路口”逻辑

    // “继续”按钮，开始倒计时
    connect(btnContinue, &QPushButton::clicked, this, [this]() {
        // 1） 隐藏三个按钮
        btnContinue->hide();
        btnRestart->hide();
        btnQuit->hide();

        // 2） 初始化并显示倒计时
        countdownValue = 3;
        countdownLabel->setText("3");
        countdownLabel->show();

        // 3） 启动倒计时闹钟，每1秒响一次
        countdownTimer->start(1000);
    });




    connect(btnRestart, &QPushButton::clicked, this, [this]() {
        // 1. 备份当前谱面路径
        QString pathBackup = this->mapPath;

        // 2. 掐断音频
        player->stop();


        // 4. 延迟50ms再去创建新界面
        QTimer::singleShot(50, [this, pathBackup]() {
            GameScene* newScene = new GameScene(pathBackup);
            newScene->show();

            // 等新窗口顺利显示出来了，再把旧窗口彻底释放掉
            this->close();
        });


    });

    connect(btnQuit, &QPushButton::clicked, this, [this]() {
        // TODO
        // 选歌界面没写好，先直接关闭。之后这里可以emit一个信号
        this->close();
    });
}


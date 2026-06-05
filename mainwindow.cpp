#include "MainWindow.h"
#include "SelectSongWindow.h"
#include <QBrush>
#include <QFont>
#include <QGraphicsTextItem>
#include <QDebug>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QDir>
#include <QUrl>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)
    , view(new QGraphicsView(this))
    , scene(new QGraphicsScene(this))
    // 初始化双播放器与双视频图层
    , m_player1(new QMediaPlayer(this))
    , m_player2(new QMediaPlayer(this))
    , m_videoItem1(new QGraphicsVideoItem())
    , m_videoItem2(new QGraphicsVideoItem())
    , m_audioOutput1(new QAudioOutput(this))
    , m_audioOutput2(new QAudioOutput(this))
    , m_isFirstVideo(true)
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->showFullScreen();

    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(view);

    // ========== Debug 信息：打印当前工作目录 ==========
    qDebug() << "=== Current working directory ===" << QDir::currentPath();

    // ========== 视频输出与图层初始化 ==========
    m_player1->setVideoOutput(m_videoItem1);
    m_player2->setVideoOutput(m_videoItem2);

    scene->addItem(m_videoItem1);
    scene->addItem(m_videoItem2);

    // 初始层级：视频 1 在前 (-1)，视频 2 在后 (-2) 潜伏
    m_videoItem1->setZValue(-1);
    m_videoItem2->setZValue(-2);

    m_videoItem1->setSize(QSizeF(width(), height()));
    m_videoItem2->setSize(QSizeF(width(), height()));

    // 音频输出静音
    m_audioOutput1->setVolume(0);
    m_audioOutput2->setVolume(0);
    m_player1->setAudioOutput(m_audioOutput1);
    m_player2->setAudioOutput(m_audioOutput2);

    // 连接播放器 1 的状态变化信号（用于视频 1 播放结束）
    connect(m_player1, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);

    // ========== 获取绝对路径 ==========
    // QFileInfo video1Info("video1.mp4");
    // QFileInfo video2Info("video2.mp4");

    // QString video1Path = video1Info.absoluteFilePath();
    // QString video2Path = video2Info.absoluteFilePath();
    // 有问题，删了

    // 检查文件是否存在
    //if (!video1Info.exists()) qDebug() << "ERROR: video1.mp4 NOT found at:" << video1Path;
    //if (!video2Info.exists()) qDebug() << "ERROR: video2.mp4 NOT found at:" << video2Path;

    // ========== 双缓冲加载与预播放 ==========
    // 1. 播放器 1 立即开始播放视频 1

    m_player1->setSource(QUrl("qrc:/movie/movies/video1.mp4")); // 已修改
    m_player1->play();

    // 2. 播放器 2 提前【预加载】视频 2，并直接设为【无限循环】
    m_player2->setSource(QUrl("qrc:/movie/movies/video2.mp4")); // 已修改

    m_player2->setLoops(QMediaPlayer::Infinite);


    view->setFocus();
}

MainWindow::~MainWindow()
{
    m_player1->stop();
    m_player2->stop();
    delete ui;
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    // qDebug() << "Player1 MediaStatus changed:" << status;

    // 当视频 1 播放到末尾时
    if (status == QMediaPlayer::EndOfMedia) {
        if (m_isFirstVideo) {
            // qDebug() << "First video finished! Seamlessly switching to video 2...";
            m_isFirstVideo = false;

            // 1. 瞬间启动早已准备就绪的视频 2
            m_player2->play();

            // 2. 瞬间调换 Z 轴层级，把视频 2 顶到最前，把视频 1 压到幕后
            m_videoItem2->setZValue(-1);
            m_videoItem1->setZValue(-2);

            // 3. 悄悄停止视频 1 以释放内存
            m_player1->stop();
        }
    }
}

void MainWindow::onSelectSong()
{

    SelectSongWindow *window = new SelectSongWindow();


    window->setAttribute(Qt::WA_DeleteOnClose);
    window->showFullScreen();

    this->deleteLater();


    delete this;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        this->close();   // 退出游戏
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 鼠标点击任何位置都进入选曲界面
    onSelectSong();
}

#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include "track.h"

class GameScene : public QWidget
{
    Q_OBJECT
private:
    // 1.游戏核心变量
    double globalSpeed;      // 全局流速 像素/ms
    qint64 currentMusicTime; // 当前音乐播放时间 ms //初始化时记得设为0
    int hitLineY;            // 判定线的统一Y坐标 像素

    // 2.四条轨道的指针
    Track* tracks[4];

    // 3.定时器，用于每隔x毫秒触发刷新
    QTimer* updateTimer;     // 负责触发每帧刷新的“闹钟”
public:

    // 0.构造函数
    // 一、为成员变量赋值
    // 二、在构造函数中设计窗口的形状，例如用 this->setFixedSize 设计大小，this->setStyleSheet 设计样式
    // 三、创建四条轨道
    // 四、设置updateTime定时器，并连接信号槽：每次定时器响就执行gameloop。定时器的间隔可以先设置为16ms，之后再调整
    explicit GameScene(QWidget *parent = nullptr);

    // 1.析构函数
    ~GameScene();
protected:
    // 2.键盘事件监听函数的重载：键盘按下 D F J K 四个键，调用对应轨道的checkHit函数
    void keyPressEvent(QKeyEvent *event) override;

    // 3.键盘事件监听函数的重载：键盘松开。
    // 此处是留给hold的逻辑，可以先不写
    void keyReleaseEvent(QKeyEvent *event) override;

    // 4.画图：这里负责画出静态的舞台，包括背景和轨道、判定线
    void paintEvent(QPaintEvent *event) override;

private slots:

    // 3.主循环，updateTimer 每隔 x ms 就会调用一次这个函数
    // 一、获取来自音乐模块的 currentMusicTime。由于音乐模块还没写，可以先按照 += 递增模拟一下
    // 二、调用轨道的 updateTrack 更新
    void gameLoop();
};

#endif // GAMESCENE_H

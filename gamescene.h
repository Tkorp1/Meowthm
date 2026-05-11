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
    qint64 currentMusicTime; // 当前音乐播放时间 ms
    int hitLineY;            // 判定线的统一 Y 坐标 像素

    // 2.四条轨道
    Track* tracks[4];

    // 3.定时器
    QTimer* updateTimer;     // 负责触发每帧刷新的“闹钟”
public:
    explicit GameScene(QWidget *parent = nullptr);

signals:
};

#endif // GAMESCENE_H

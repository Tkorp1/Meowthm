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

    // 4.记分系统与ui控件
    int currentScore; // 当前得分
    int currentCombo; // 当前连击数（注意miss时要清零）
    QLabel* scoreLabel; // 显示分数的标签
    QLabel* combolabel; // 显示 combo 的标签

    // 5.音乐引擎（暂时先不写）


public:

    // 0.构造函数
    // 一、为成员变量赋值
    // 二、在构造函数中设计窗口的形状，例如用 this->setFixedSize 设计大小，this->setStyleSheet 设计样式；设计分数空间的位置和样式（注意：要把分数控件移动到最顶层）
    // 三、创建四条轨道，并把它们的信号连接到 hitNoteJudge 函数上
    // 四、设置updateTime定时器，并连接信号槽：每次定时器响就执行gameloop。定时器的间隔可以先设置为16ms，之后再调整
    // 五、使用谱面解析器生成音符（谱面解析器还没写，这一块暂）
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

    // 5.主循环，updateTimer 每隔 x ms 就会调用一次这个函数
    // 一、获取来自音乐模块的 currentMusicTime。由于音乐模块还没写，可以先按照 += 递增模拟一下
    // 二、调用轨道的 updateTrack 更新
    void gameLoop();


    // 6.接受来自track的判定结果，并对 currentScore 和 currentCombo 做出更改
    // 在track中已经约定： 0 空 、1 miss 、2 good 、3 perfect
    void hitNoteJudge(int result);
};


#endif // GAMESCENE_H

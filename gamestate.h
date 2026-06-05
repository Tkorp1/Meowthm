#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <QString>
#include <QVector>
#include <QList>
// 存放所有和游戏相关的数据

class GameState
{
private:
    // 1.当前歌曲，为str，缺省值为None
    QString currentSong;

    // 2.当前总分
    int currentScore;

    // 3.当前accuracy，是一个小于等于一的浮点数
    double currentAcc;

    // 4.当前combo
    int currentCombo;

    // 5.最大combo
    int maxCombo;

    // 6.当前p总数
    int currentPerfect;

    // 7.当前g总数
    int currentGood;

    // 8.当前m总数
    int currentMiss;

    // 9.存放所有note的判定，按照时间顺序（因为物量都为1）
    QVector<int> allJudge;

    // 10.数据分析
    QList<qint64> m_hitOffsets;     // 记录每一次敲击的误差(deltaT)
    QList<qint64> m_hitTimestamps;  // 记录每一次敲击发生的时间戳
    int m_leftHandHits = 0;         // 左半脑(左手)负载
    int m_rightHandHits = 0;        // 右半脑(右手)负载

public:
    // 1.构造函数
    GameState(QString _currentSong = "None");// 一开始只有歌曲参数

    // 2.所有的读取函数

    QString getCurrentSong() const;

    int getCurrentScore() const;

    double getCurrentAcc() const;

    int getCurrentCombo() const;

    int getMaxCombo() const;

    int getCurrentPerfect() const;

    int getCurrentGood() const;

    int getCurrentMiss() const;

    QVector<int> getAllJudge() const;

    // 3.修改状态函数
    void changeCurrentState(int judge);

    void recordHit(int trackId, qint64 deltaT, qint64 musicTime);

    // 【新增】：数据读取接口（之后画图用）
    QList<qint64> getHitOffsets() const;
    QList<qint64> getHitTimestamps() const;
    int getLeftHandHits() const;
    int getRightHandHits() const;
};

#endif // GAMESTATE_H

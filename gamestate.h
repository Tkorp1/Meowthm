#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <QString>
#include <QVector>
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

public:
    // 1.构造函数
    GameState(QString _currentSong);// 一开始只有歌曲参数

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
};

#endif // GAMESTATE_H

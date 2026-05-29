#ifndef GAMECONFIG_H
#define GAMECONFIG_H
#include <QString>
// 这里面储存的都是全局变量，设想是在gamescene初始化或者main里面就将这个设置好
// 有很多接口，可以用get读取和set修改

class GameConfig
{
private:
    // 1.谱面流速
    double noteSpeed;

    // 2.当前玩家名
    QString currentPlayer;

    // 3.当前偏移值
    qint64 currentOffset;

public:
    // 1.构造函数
    GameConfig(double _noteSpeed, QString _currentPlayer, qint64 _currentOffset);

    // 2.一些读取函数
    double getNoteSpeed() const;

    QString getCurrentPlayer() const;

    qint64 getCurrentOffset() const;

    // 3.一些修改函数
    void setNoteSpeed(double newSpeed);

    void setCurrentPlayer(QString newName);

    void setCurrentOffset(qint64 newOffset);
};

#endif // GAMECONFIG_H

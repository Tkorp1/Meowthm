#ifndef MAPPARSER_H
#define MAPPARSER_H

#include "note.h"
#include "track.h"
#include<QString>

class GameScene;


class MapParser
{
protected:
    int currentBPM; // 记录曲目bpm

    // gpt说目前不需要gamescene所以先不管（）
    // GameScene* currentGameScene; // 记录当前游戏界面，方便给新生成的音符 parent

    Track** ptrTracks; // 指向四个轨道的指针
    QString Path;//记录歌曲名的路径
public:
    MapParser(QString _path);
    // 获取轨道的note函数，通过track路径
    QList<Note*> parse(QString trackPath);

    // 计算音符的时间的函数，参数为小节数（1开始）和节拍数
    qint64 targetTimeCalculator(int bar, double beats);

};

#endif // MAPPARSER_H

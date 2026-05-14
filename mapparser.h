#ifndef MAPPARSER_H
#define MAPPARSER_H

#include "note.h"
#include "track.h"

class GameScene;


class MapParser
{
protected:
    int currentBPM; // 记录曲目bpm
    GameScene* currentGameScene; // 记录当前游戏界面，方便给新生成的音符 parent
    Track** ptrTracks; // 指向四个轨道的指针
public:
    MapParser();
};

#endif // MAPPARSER_H

#include "gameconfig.h"

// 1.构造函数
GameConfig::GameConfig(double _noteSpeed, QString _currentPlayer, qint64 _currentOffset):
    noteSpeed(_noteSpeed), currentPlayer(_currentPlayer), currentOffset(_currentOffset)
{

}

// 2.一些读取函数
double GameConfig::getNoteSpeed() const{
    return noteSpeed;
}

QString GameConfig::getCurrentPlayer() const{
    return currentPlayer;
}

qint64 GameConfig::getCurrentOffset() const{
    return currentOffset;
}

// 3.一些修改函数
void GameConfig::setNoteSpeed(double newSpeed){
    noteSpeed = newSpeed;
    return;
}

void GameConfig::setCurrentPlayer(QString newName){
    currentPlayer = newName;
    return;
}

void GameConfig::setCurrentOffset(qint64 newOffset){
    currentOffset = newOffset;
    return;
}
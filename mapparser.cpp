#include "mapparser.h"
#include "gamescene.h"
MapParser::MapParser(QString _path, GameScene* _currentGameScene):Path(_path) {
    currentGameScene = _currentGameScene;
}
QList<Note*> MapParser::parse(){

}
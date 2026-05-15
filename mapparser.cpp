#include<QFile>
#include<QString>
#include<QTextStream>
#include<QDir>
#include<QDebug>
#include "mapparser.h"
#include "gamescene.h"
MapParser::MapParser(QString _path):Path(_path)
{

}
QList<Note*> MapParser::parse(QString trackPath){
    QDir dir(Path);

}
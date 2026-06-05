#include<QFile>
#include<QString>
#include<QTextStream>
#include<QDir>
#include<QDebug>
#include "mapparser.h"
#include "gameconfig.h"
MapParser::MapParser(QString _path):Path(_path)
{
    // 这里需要通过这个文件路径来获取BPM：
    QDir dir(Path);
    QString infoPath = dir.filePath("info.txt");
    QFile file(infoPath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "打不开 info 文件";
        return;
    }

    QTextStream in(&file);


    // 读整行

    QString line = in.readLine();

    // 按 ":" 分割

    QStringList parts = line.split(":");

    if (parts.size() == 2) {

        currentBPM = parts[1].trimmed().toInt();

    } else {

        qDebug() << "BPM格式错误";

    }
}
qint64 MapParser::targetTimeCalculator(double beats){
    beats--;

    double result =beats * (60000.0) / currentBPM;

    return qRound64(result);

}

QList<Note*> MapParser::parse(QString trackPath){
    QList<Note*> result;
    QFile file(trackPath);
    if (!file.open(QIODevice::ReadOnly)) {

        qDebug() << "打开失败:" << trackPath;

        return result;

    }

    QTextStream in(&file);
    //现在开始就可以读文件了
    while(!in.atEnd()){
        int type=-1;//默认值
        // note样式
        int noteWidth = 100;
        int noteHeight = 30;
        in>>type;


        // 保险机制，防止没读到
        if(type == -1){
            break;
        }

        if(type==0){
            /*
             现在是note
             需要读取的是一个double类型的数表示多少拍。
            */
            double beats;
            in>>beats;

            // 下面开始初始化一个note

            qint64 _targetTime = targetTimeCalculator(beats);

            Note* noteTemp = new Tap(_targetTime, noteWidth, noteHeight);
            result.append(noteTemp);


        }
        else if(type==1){
            //现在是hold
            double beginBeats, endBeats;
            in>>beginBeats>>endBeats;
            // 下面初始化一个hold

            qint64 _targettimeBegin = targetTimeCalculator(beginBeats);
            qint64 _targettimeEnd = targetTimeCalculator(endBeats);

            double durationMs = _targettimeEnd - _targettimeBegin;
            double currentSpeed = GameConfig::instance()->getNoteSpeed();
            int holdHeight = qRound(durationMs * currentSpeed);

            Note* noteTemp = new Hold(_targettimeBegin, _targettimeEnd, noteWidth, holdHeight);
            result.append(noteTemp);

        }

    }
    return result;

}

qint64 MapParser::getMusicTime(){
    QDir dir(Path);

    QString infoPath = dir.filePath("info.txt");

    QFile file(infoPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "打不开 info 文件";
        return 0;
    }

    QTextStream in(&file);

    in.readLine();
    in.readLine();

    // 读整行

    QString line = in.readLine();

    file.close();

    QStringList parts = line.split(":");

    if(parts.size() < 2){

        return 0;

    }

    return parts[1].trimmed().toLongLong();

}

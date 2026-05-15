#include<QFile>
#include<QString>
#include<QTextStream>
#include<QDir>
#include<QDebug>
#include "mapparser.h"
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

qint64 MapParser::targetTimeCalculator(int bar, double beats){
    bar--;// 首先就将bar换成0开始的索引

    //听gpt的先加一个每小节的节拍数方便修改

    int beatsPerBar = 4;

    //下面的时间都是一个以毫秒为单位的数字
    double timeOfABeat = (60000.0)/currentBPM;

    double timeOfABar = beatsPerBar * timeOfABeat;

    double result = bar*timeOfABar + beats * timeOfABeat;

    qint64 resulttime = qRound64(result);

    return resulttime;

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
        int type=0;//默认值
        in>>type;
        if(type==0){
            /*
             现在是note
             需要读取的是两个数字，一个整数代表第几小节（从1开始，所以要注意-1）
             第二个是一个double类型的数，0-4，表示这个小节的多少拍。
             比如第一拍的后面这个八分音符就是：
             0 1 0.5
             比如第三小节第四拍的一个四分音符就是：
             0 3 3.0
            */
            int barIndex;
            double beats;
            in>>barIndex>>beats;

            // 下面开始初始化一个note

            qint64 _targetTime = targetTimeCalculator(barIndex, beats);



        }
        else if(type==1){
            //现在是hold

        }

    }

}
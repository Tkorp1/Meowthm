#include<QFile>
#include<QString>
#include<QTextStream>
#include<QDir>
#include<QDebug>
#include "mapparser.h"
#include "gameconfig.h"

MapParser::MapParser(QString _path):Path(_path)
{
    QDir dir(Path);
    QString infoPath = dir.filePath("info.txt");
    QFile file(infoPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "打不开 info 文件";
        return;
    }
    QTextStream in(&file);
    QString line = in.readLine();
    QStringList parts = line.split(":");
    if (parts.size() == 2) {
        currentBPM = parts[1].trimmed().toInt();
    } else {
        qDebug() << "BPM格式错误";
    }
}

qint64 MapParser::targetTimeCalculator(double beats)
{
    beats--;  // 拍数从1开始，转换为0起始
    double result = beats * (60000.0) / currentBPM;
    return qRound64(result);
}

QList<Note*> MapParser::parse(QString trackPath)
{
    QList<Note*> result;
    QFile file(trackPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "打开失败:" << trackPath;
        return result;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        int type = -1;
        in >> type;
        if (type == -1 || in.status() != QTextStream::Ok) break;

        const int noteWidth = 92;
        const int noteHeight = 25;

        if (type == 0) {
            double beats;
            in >> beats;
            qint64 _targetTime = targetTimeCalculator(beats);
            Note* noteTemp = new Tap(_targetTime, noteWidth, noteHeight);
            result.append(noteTemp);
        }
        else if (type == 1) {
            double beginBeats, endBeats;
            in >> beginBeats >> endBeats;
            qint64 _targettimeBegin = targetTimeCalculator(beginBeats);
            qint64 _targettimeEnd = targetTimeCalculator(endBeats);
            double durationMs = _targettimeEnd - _targettimeBegin;
            double currentSpeed = GameConfig::instance()->getNoteSpeed();
            int holdHeight = qRound(durationMs * currentSpeed);
            // 防止负高度或零高度
            if (holdHeight < 1) holdHeight = 1;
            Note* noteTemp = new Hold(_targettimeBegin, _targettimeEnd, noteWidth, holdHeight);
            result.append(noteTemp);
        }
        else {
            qDebug() << "未知音符类型" << type << "in" << trackPath;
            continue;  // 跳过未知行
        }
    }
    qDebug() << "Track file:" << trackPath << "notes:" << result.size();
    return result;
}

qint64 MapParser::getMusicTime()
{
    QDir dir(Path);
    QString infoPath = dir.filePath("info.txt");
    QFile file(infoPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "打不开 info 文件";
        return 0;
    }
    QTextStream in(&file);
    in.readLine(); // BPM
    in.readLine(); // songname
    QString line = in.readLine(); // songlength
    file.close();
    QStringList parts = line.split(":");
    if (parts.size() < 2) return 0;
    return parts[1].trimmed().toLongLong();
}

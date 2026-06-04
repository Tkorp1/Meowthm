#include "gameconfig.h"
#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QFile>

GameConfig* GameConfig::instance()
{
    static GameConfig inst;
    return &inst;
}

GameConfig::GameConfig(QObject *parent) : QObject(parent)
{
    QSettings settings;
    m_noteSpeed = settings.value("game/noteSpeed", 0.5).toDouble();
    m_currentPlayer = settings.value("game/currentPlayer", "Player").toString();
    m_currentOffset = settings.value("game/currentOffset", 0).toLongLong();

    // 加载歌曲列表
    loadSongs();

    qDebug() << "GameConfig initialized: speed=" << m_noteSpeed
             << "player=" << m_currentPlayer
             << "offset=" << m_currentOffset
             << "songs count=" << m_songs.size();
}

GameConfig::~GameConfig()
{
    QSettings settings;
    settings.setValue("game/noteSpeed", m_noteSpeed);
    settings.setValue("game/currentPlayer", m_currentPlayer);
    settings.setValue("game/currentOffset", m_currentOffset);
}

void GameConfig::setNoteSpeed(double speed)
{
    if (qFuzzyCompare(m_noteSpeed, speed))
        return;
    m_noteSpeed = speed;
    QSettings().setValue("game/noteSpeed", speed);
    emit noteSpeedChanged(speed);
}

void GameConfig::setCurrentPlayer(const QString &name)
{
    if (m_currentPlayer == name)
        return;
    m_currentPlayer = name;
    QSettings().setValue("game/currentPlayer", name);
    emit currentPlayerChanged(name);
}

void GameConfig::setCurrentOffset(qint64 offset)
{
    if (m_currentOffset == offset)
        return;
    m_currentOffset = offset;
    QSettings().setValue("game/currentOffset", offset);
    emit currentOffsetChanged(offset);
}

void GameConfig::loadSongs()
{
    m_songs.clear();
    // 假设谱面文件夹放在程序运行目录下的 "maps" 文件夹中
    QDir mapsDir("maps");
    if (!mapsDir.exists()) {
        qDebug() << "maps directory not found, no songs loaded.";
        return;
    }

    // 获取所有子目录
    QStringList subDirs = mapsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &dirName : subDirs) {
        QString folderPath = mapsDir.absoluteFilePath(dirName);
        // 简单检查：是否存在 info.txt 和 music.mp3（可选）
        QFile infoFile(folderPath + "/info.txt");
        QFile musicFile(folderPath + "/music.mp3");
        if (!infoFile.exists() && !musicFile.exists()) {
            qDebug() << "Skipping" << dirName << ": missing info.txt or music.mp3";
            continue;
        }

        // 读取 info.txt 的第一行获取 BPM，第二行获取歌曲名？根据文档，info.txt 格式：
        // songBPM:default
        // songname:default
        // songlength:default
        // 我们可以从第二行读取歌曲名
        QString songName = dirName; // 默认使用文件夹名
        if (infoFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&infoFile);
            in.readLine(); // 跳过 BPM 行
            QString line = in.readLine(); // 歌曲名行
            if (line.startsWith("songname:")) {
                songName = line.mid(9).trimmed();
            }
            infoFile.close();
        }

        SongInfo info;
        info.name = songName;
        info.coverPath = folderPath + "/cover.jpg";  // 可选，如果存在
        info.mapFolderPath = folderPath;
        m_songs.append(info);
        qDebug() << "Loaded song:" << songName << "path:" << folderPath;
    }
}

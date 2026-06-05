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
    m_noteSpeed = settings.value("game/noteSpeed", 1).toDouble();
    m_currentPlayer = settings.value("game/currentPlayer", "Player").toString();
    m_currentOffset = settings.value("game/currentOffset", 0).toLongLong();
    m_hitSoundVolume = settings.value("game/hitSoundVolume", 100).toInt();

    m_tracksCleared = settings.value("stats/tracksCleared", 0).toInt();
    m_fullCombos = settings.value("stats/fullCombos", 0).toInt();
    m_totalAccuracySum = settings.value("stats/totalAccuracySum", 0.0).toDouble();
    m_peakKPS = settings.value("stats/peakKPS", 0).toInt();
    m_totalPlayTimeSec = settings.value("stats/totalPlayTimeSec", 0).toLongLong();

    m_playerBio = settings.value("game/playerBio", "这个人很神秘，还没有留下打歌记录...").toString();

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
    settings.setValue("game/hitSoundVolume", m_hitSoundVolume);
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


    // 注意：谱面文件夹现在存放在 .qrc 资源文件的 ":/map/maps" 目录下！！！！！！！
    // ⚠️⚠️⚠️一定注意！！！

    QDir mapsDir(":/map/maps");

    if (!mapsDir.exists()) {
        qDebug() << " maps directory not found in qrc, no songs loaded.";
        return;
    }

    // 获取所有子目录（比如 song1, song2 等）
    QStringList subDirs = mapsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &dirName : subDirs) {
        // 获取绝对资源路径，例如 ":/map/maps/song1"
        QString folderPath = mapsDir.absoluteFilePath(dirName);

        // 检查是否存在 info.txt 和 music.mp3

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
        // 如果资源里以后加了 cover.jpg，也能自动读取并显示在选曲界面上
        info.coverPath = folderPath + "/cover.jpg";
        // 传给 GameScene 的路径，正是我们之前兼容好的 ":/map/maps/song1"
        info.mapFolderPath = folderPath;

        m_songs.append(info);
        qDebug() << "Loaded song from qrc:" << songName << "path:" << folderPath;

    }
}

void GameConfig::setHitSoundVolume(int volume)
{
    if (m_hitSoundVolume == volume) return;
    m_hitSoundVolume = volume;
    QSettings().setValue("game/hitSoundVolume", volume);
    emit hitSoundVolumeChanged(volume); // 发出信号，通知全服！
}

// ==========================================
// 写入作战记录
// ==========================================
void GameConfig::addCombatRecord(const GameState& state)
{
    // 1. 基础数据累计
    m_tracksCleared++;
    if (state.getCurrentMiss() == 0) {
        m_fullCombos++; // 没有 Miss，就是 Full Combo！
    }
    m_totalAccuracySum += state.getCurrentAcc();

    // 2. 解析反射时间戳，计算高阶数据
    QList<qint64> times = state.getHitTimestamps();

    // 突破极限计算：这局的 KPS 有没有打破历史最高记录？
    int sessionPeakKPS = 0;
    for (int i = 0; i < times.size(); ++i) {
        int currentKPS = 0;
        for (int j = i; j < times.size(); ++j) {
            if (times[j] - times[i] <= 1000) currentKPS++;
            else break;
        }
        if (currentKPS > sessionPeakKPS) sessionPeakKPS = currentKPS;
    }
    if (sessionPeakKPS > m_peakKPS) {
        m_peakKPS = sessionPeakKPS; // 破纪录啦！
    }

    // 计算这首歌的实际游玩时长 (最后一个音符减去第一个音符)
    if (times.size() >= 2) {
        qint64 durationMs = times.last() - times.first();
        m_totalPlayTimeSec += (durationMs / 1000);
    }

    // 3. 立即存入本地注册表，防止游戏崩溃丢失数据！
    QSettings settings;
    settings.setValue("stats/tracksCleared", m_tracksCleared);
    settings.setValue("stats/fullCombos", m_fullCombos);
    settings.setValue("stats/totalAccuracySum", m_totalAccuracySum);
    settings.setValue("stats/peakKPS", m_peakKPS);
    settings.setValue("stats/totalPlayTimeSec", m_totalPlayTimeSec);
}


// 【新增】：将简介保存到本地注册表/配置中
void GameConfig::setPlayerBio(const QString &bio)
{
    if (m_playerBio == bio)
        return;
    m_playerBio = bio;
    QSettings().setValue("game/playerBio", bio); // 写进硬盘！
}

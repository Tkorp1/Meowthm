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

    // 【新增】：读取硬盘里的成就数据
    int achCount = settings.beginReadArray("achievements");
    for (int i = 0; i < achCount; ++i) {
        settings.setArrayIndex(i);
        m_unlockedAchievements.insert(settings.value("id").toInt());
    }
    settings.endArray();



    qDebug() << "GameConfig initialized: speed=" << m_noteSpeed
             << "player=" << m_currentPlayer
             << "offset=" << m_currentOffset;
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


void GameConfig::setHitSoundVolume(int volume)
{
    if (m_hitSoundVolume == volume) return;
    m_hitSoundVolume = volume;
    QSettings().setValue("game/hitSoundVolume", volume);
    emit hitSoundVolumeChanged(volume); // 发出信号，通知全服！
}

// ==========================================
// 写入打歌记录
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

    // 每次更新完作战记录，立刻看一下有没有解锁新成就！
    checkAchievements(state);
}


// 【新增】：将简介保存到本地注册表/配置中
void GameConfig::setPlayerBio(const QString &bio)
{
    if (m_playerBio == bio)
        return;
    m_playerBio = bio;
    QSettings().setValue("game/playerBio", bio); // 写进硬盘！
}

// ==========================================
// 成就系统：判断与解锁逻辑
// ==========================================
bool GameConfig::isAchievementUnlocked(int id) const {
    return m_unlockedAchievements.contains(id);
}

void GameConfig::unlockAchievement(int id) {
    // 如果还没解锁，就解锁并立即存入硬盘！
    if (!m_unlockedAchievements.contains(id)) {
        m_unlockedAchievements.insert(id);

        QSettings settings;
        settings.beginWriteArray("achievements");
        int i = 0;
        for (int achId : std::as_const(m_unlockedAchievements)) {
            settings.setArrayIndex(i++);
            settings.setValue("id", achId);
        }
        settings.endArray();

        qDebug() << "🏆 恭喜解锁成就 ID:" << id;
        // （未来这里可以加一个 emit 信号，在游戏里弹出一个“成就解锁”的横幅！）
    }
}

void GameConfig::checkAchievements(const GameState& state) {
    // 1. 滚木：在一首歌曲里得分为 0
    if (state.getCurrentScore() == 0) unlockAchievement(1);

    // 2. 打一首歌：有手就行
    if (m_tracksCleared >= 1) unlockAchievement(2);

    // 基于评级的成就（必须打过歌才算）
    if (m_tracksCleared > 0) {
        double avgAcc = getAverageAccuracy() * 100.0;

        // 3. 菜就多练：C及以下 (Acc < 80.0)
        if (avgAcc < 80.0) unlockAchievement(3);

        // 4. 我承认你有点鸟不起：B及以上 (Acc >= 80.0)
        if (avgAcc >= 80.0) unlockAchievement(4);

        // 5. 大神啊：A及以上 (Acc >= 90.0)
        if (avgAcc >= 90.0) unlockAchievement(5);

        // 6. 令 人 忍 俊 不 禁：S+及以上 (Acc >= 98.0)
        if (avgAcc >= 98.0) unlockAchievement(6);
    }
}

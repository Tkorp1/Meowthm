#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSet>

#include "gamestate.h" // 【新增】因为我们要解析战斗数据



struct SongInfo {
    QString name;           // 歌曲名称（用于显示）
    QString coverPath;      // 封面图片路径（相对路径，可选）
    QString mapFolderPath;  // 谱面文件夹绝对路径或相对路径（相对于可执行文件）
};

class GameConfig : public QObject
{
    Q_OBJECT

public:
    static GameConfig* instance(); // 这是一个全局单例


    double getNoteSpeed() const { return m_noteSpeed; } // 流速
    void setNoteSpeed(double speed);

    QString getCurrentPlayer() const { return m_currentPlayer; } // 玩家名字
    void setCurrentPlayer(const QString &name);

    qint64 getCurrentOffset() const { return m_currentOffset; } // 偏移
    void setCurrentOffset(qint64 offset);


    int getHitSoundVolume() const { return m_hitSoundVolume; }
    void setHitSoundVolume(int volume);


    int getTracksCleared() const { return m_tracksCleared; }
    int getFullCombos() const { return m_fullCombos; }
    double getAverageAccuracy() const {
        return m_tracksCleared > 0 ? (m_totalAccuracySum / m_tracksCleared) : 0.0;
    }
    int getPeakKPS() const { return m_peakKPS; }
    qint64 getTotalPlayTimeSec() const { return m_totalPlayTimeSec; }

    // 打完一首歌后，向系统上报数据的接口！
    void addCombatRecord(const GameState& state);

    QString getPlayerBio() const { return m_playerBio; }  // 【新增】获取简介
    void setPlayerBio(const QString &bio);

    // 成就系统的三大核心接口
    bool isAchievementUnlocked(int id) const;
    void unlockAchievement(int id);
    void checkAchievements(const GameState& state); // 每次打完歌触发审核


signals:
    void noteSpeedChanged(double newSpeed);
    void currentPlayerChanged(const QString &newName);
    void currentOffsetChanged(qint64 newOffset);
    void hitSoundVolumeChanged(int volume);

private:
    explicit GameConfig(QObject *parent = nullptr);
    ~GameConfig();
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    double m_noteSpeed;
    QString m_currentPlayer;
    qint64 m_currentOffset;


    int m_hitSoundVolume;

    int m_tracksCleared = 0;
    int m_fullCombos = 0;
    double m_totalAccuracySum = 0.0;
    int m_peakKPS = 0;
    qint64 m_totalPlayTimeSec = 0;

    QString m_playerBio;

    QSet<int> m_unlockedAchievements; // 存储已解锁成就的 ID
};

#endif // GAMECONFIG_H

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QObject>
#include <QString>
#include <QList>

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

    // 歌曲列表接口
    QList<SongInfo> getSongs() const { return m_songs; }
    void loadSongs();   // 加载歌曲列表（可扫描 maps 目录或读取配置文件）

signals:
    void noteSpeedChanged(double newSpeed);
    void currentPlayerChanged(const QString &newName);
    void currentOffsetChanged(qint64 newOffset);

private:
    explicit GameConfig(QObject *parent = nullptr);
    ~GameConfig();
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    double m_noteSpeed;
    QString m_currentPlayer;
    qint64 m_currentOffset;

    QList<SongInfo> m_songs;
};

#endif // GAMECONFIG_H

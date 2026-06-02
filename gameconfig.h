#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QObject>
#include <QString>

// 全局配置类（单例模式），用于存储游戏相关的全局设置
// 用法：GameConfig::instance()->getNoteSpeed();
//       GameConfig::instance()->setNoteSpeed(1.2);
class GameConfig : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static GameConfig* instance();

    // 获取/设置 流速（像素/毫秒）
    double getNoteSpeed() const { return m_noteSpeed; }
    void setNoteSpeed(double speed);

    // 获取/设置 当前玩家昵称
    QString getCurrentPlayer() const { return m_currentPlayer; }
    void setCurrentPlayer(const QString &name);

    // 获取/设置 偏移量（毫秒）
    qint64 getCurrentOffset() const { return m_currentOffset; }
    void setCurrentOffset(qint64 offset);

signals:
    void noteSpeedChanged(double newSpeed);
    void currentPlayerChanged(const QString &newName);
    void currentOffsetChanged(qint64 newOffset);

private:
    // 私有构造/析构，禁止外部创建
    explicit GameConfig(QObject *parent = nullptr);
    ~GameConfig();

    // 禁用拷贝和赋值
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    // 数据成员
    double m_noteSpeed;
    QString m_currentPlayer;
    qint64 m_currentOffset;
};

#endif // GAMECONFIG_H

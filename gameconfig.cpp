#include "gameconfig.h"
#include <QSettings>
#include <QDebug>

GameConfig* GameConfig::instance() // 全局单例实现存储游戏的全局变量
{
    static GameConfig inst;
    return &inst;
}

GameConfig::GameConfig(QObject *parent) : QObject(parent)
{
    // 从持久化存储中加载数据
    QSettings settings;
    m_noteSpeed = settings.value("game/noteSpeed", 4).toDouble();
    m_currentPlayer = settings.value("game/currentPlayer", "Player").toString();
    m_currentOffset = settings.value("game/currentOffset", 0).toLongLong();

    qDebug() << "GameConfig initialized: speed=" << m_noteSpeed
             << "player=" << m_currentPlayer
             << "offset=" << m_currentOffset;
}

GameConfig::~GameConfig()
{
    // 可以保存最后一次的值（QSettings 已经在上面的 setter 中保存，但析构时再确保一次）
    QSettings settings;
    settings.setValue("game/noteSpeed", m_noteSpeed);
    settings.setValue("game/currentPlayer", m_currentPlayer);
    settings.setValue("game/currentOffset", m_currentOffset);
}

void GameConfig::setNoteSpeed(double speed) // 流速
{
    if (qFuzzyCompare(m_noteSpeed, speed)) // 忽略浮点数误差
        return;
    m_noteSpeed = speed;
    QSettings().setValue("game/noteSpeed", speed); // 保存流速
    emit noteSpeedChanged(speed); // 发射信号：流速改变了，预览及时更改流速
}

void GameConfig::setCurrentPlayer(const QString &name) // 玩家昵称
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

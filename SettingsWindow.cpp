#include "SettingsWindow.h"
#include "gameconfig.h"          // 全局配置单例
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

// ==================== PreviewTrack 实现 ====================
PreviewTrack::PreviewTrack(QWidget *parent)
    : QWidget(parent)
    , m_speed(4)          // 默认速度，与 GameConfig 初始值一致
    , m_noteY(0)
    , m_timerId(0)
{
    setFixedSize(100, 300);          // 轨道宽度100，高度300
    setStyleSheet("background-color: rgba(0,0,0,100); border: 1px solid white;");

    // 启动定时器（每16ms一帧，约60fps）
    m_timerId = startTimer(16);
}

PreviewTrack::~PreviewTrack()
{
    if (m_timerId)
        killTimer(m_timerId);
}

void PreviewTrack::setSpeed(double speed)
{
    m_speed = speed;
}

void PreviewTrack::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制轨道线
    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(width()/2, 0, width()/2, height());

    // 绘制判定线（底部附近）
    int hitLineY = height() - 30;
    painter.setPen(QPen(Qt::yellow, 3));
    painter.drawLine(10, hitLineY, width()-10, hitLineY);

    // 绘制音符（蓝色圆形）
    painter.setBrush(QBrush(QColor(0, 191, 255)));
    painter.setPen(Qt::NoPen);
    int noteRadius = 15;
    painter.drawEllipse(QPointF(width()/2, m_noteY), noteRadius, noteRadius);
}

void PreviewTrack::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    // 每帧移动距离 = 速度(像素/毫秒) * 时间间隔(16ms)
    m_noteY += m_speed * 16;

    // 超出底部时重置到顶部，并随机或固定偏移（模拟重复下落）
    if (m_noteY > height() - 15) {
        m_noteY = 15;   // 重置到顶部附近
    }
    // 触发重绘
    update();
}

// ==================== SettingsWindow 实现 ====================
SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
{
    // 窗口基本设置
    setFixedSize(1200, 800);
    setWindowTitle("设置");

    // ---------- 1. 流速调节区域 ----------
    // 减流速按钮（-0.05）
    m_speedMinusBtn = new QPushButton("-0.05", this);
    m_speedMinusBtn->setGeometry(100, 100, 80, 40);
    m_speedMinusBtn->setStyleSheet("background-color: #4682B4; color: white; font-size: 14px;");
    connect(m_speedMinusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(-0.05); });

    // 流速显示标签
    m_speedLabel = new QLabel(this);
    m_speedLabel->setGeometry(200, 100, 100, 40);
    m_speedLabel->setAlignment(Qt::AlignCenter);
    m_speedLabel->setStyleSheet("background-color: rgba(0,0,0,150); color: white; font-size: 16px;");
    // 从全局配置读取当前流速并显示
    double curSpeed = GameConfig::instance()->getNoteSpeed();
    m_speedLabel->setText(QString::number(curSpeed, 'f', 2));

    // 加速流速按钮（+0.05）
    m_speedPlusBtn = new QPushButton("+0.05", this);
    m_speedPlusBtn->setGeometry(320, 100, 80, 40);
    m_speedPlusBtn->setStyleSheet("background-color: #4682B4; color: white; font-size: 14px;");
    connect(m_speedPlusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(0.05); });

    // ---------- 2. 偏移值调节区域 ----------
    // 减偏移按钮（-5 ms）
    m_offsetMinusBtn = new QPushButton("-5 ms", this);
    m_offsetMinusBtn->setGeometry(100, 200, 80, 40);
    m_offsetMinusBtn->setStyleSheet("background-color: #4682B4; color: white; font-size: 14px;");
    connect(m_offsetMinusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(-5); });

    // 偏移显示标签
    m_offsetLabel = new QLabel(this);
    m_offsetLabel->setGeometry(200, 200, 100, 40);
    m_offsetLabel->setAlignment(Qt::AlignCenter);
    m_offsetLabel->setStyleSheet("background-color: rgba(0,0,0,150); color: white; font-size: 16px;");
    // 从全局配置读取当前偏移值
    qint64 curOffset = GameConfig::instance()->getCurrentOffset();
    m_offsetLabel->setText(QString::number(curOffset));

    // 加偏移按钮（+5 ms）
    m_offsetPlusBtn = new QPushButton("+5 ms", this);
    m_offsetPlusBtn->setGeometry(320, 200, 80, 40);
    m_offsetPlusBtn->setStyleSheet("background-color: #4682B4; color: white; font-size: 14px;");
    connect(m_offsetPlusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(5); });

    // ---------- 3. 轨道预览 ----------
    m_previewTrack = new PreviewTrack(this);
    m_previewTrack->setGeometry(500, 200, 120, 400);
    // 连接流速变化信号，实时更新预览速度
    connect(GameConfig::instance(), &GameConfig::noteSpeedChanged,
            m_previewTrack, &PreviewTrack::setSpeed);
    // 初始设置一次速度
    m_previewTrack->setSpeed(GameConfig::instance()->getNoteSpeed());

    // ---------- 4. 背景音乐 ----------
    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    // 假设BGM文件放在工作目录下的 "setting_bgm.mp3"（用户自己放）
    m_player->setSource(QUrl::fromLocalFile("setting_bgm.mp3"));
    m_player->setLoops(-1);  // 无限循环
    m_player->play();

    // ---------- 5. 退出按钮 ----------
    m_exitBtn = new QPushButton("退出", this);
    m_exitBtn->setGeometry(1050, 700, 100, 50);
    m_exitBtn->setStyleSheet("background-color: #4682B4; color: white; font-size: 16px; border-radius: 8px;");
    connect(m_exitBtn, &QPushButton::clicked, this, &SettingsWindow::onExitClicked);
}

SettingsWindow::~SettingsWindow()
{
    // 停止并释放音乐资源
    m_player->stop();
}

void SettingsWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg("settingsbg.png");   // 背景图（可更换，请确保文件存在）
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), QColor(30, 30, 40));
        qDebug() << "未找到背景图，使用纯色背景";
    }
    QWidget::paintEvent(event);
}

// 流速调节槽函数
void SettingsWindow::onSpeedButtonClicked(double delta)
{
    double newSpeed = GameConfig::instance()->getNoteSpeed() + delta;
    // 限制流速范围（0.2 ~ 2.0）
    if (newSpeed < 0.2) newSpeed = 0.2;
    if (newSpeed > 2.0) newSpeed = 2.0;
    GameConfig::instance()->setNoteSpeed(newSpeed);
    // 显示标签的更新由 updateSpeedLabel 完成

    updateSpeedLabel(newSpeed);
}

// 偏移值调节槽函数
void SettingsWindow::onOffsetButtonClicked(qint64 delta)
{
    qint64 newOffset = GameConfig::instance()->getCurrentOffset() + delta;
    // 偏移范围可以设为 -500 ~ 500
    if (newOffset < -500) newOffset = -500;
    if (newOffset > 500) newOffset = 500;
    GameConfig::instance()->setCurrentOffset(newOffset);
    updateOffsetLabel(newOffset);
}

// 更新流速显示（也可以直接连接 GameConfig::noteSpeedChanged 信号）
void SettingsWindow::updateSpeedLabel(double speed)
{
    m_speedLabel->setText(QString::number(speed, 'f', 2));
}

void SettingsWindow::updateOffsetLabel(qint64 offset)
{
    m_offsetLabel->setText(QString::number(offset));
}

void SettingsWindow::onExitClicked()
{
    // 停止BGM
    m_player->stop();
    // 关闭当前窗口
    this->close();
    // 如果主窗口被隐藏了，显示它
    if (parentWidget()) {
        parentWidget()->show();
    }
}

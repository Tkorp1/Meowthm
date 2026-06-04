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
#include <QKeyEvent>

// ==================== PreviewTrack 实现 ====================
PreviewTrack::PreviewTrack(QWidget *parent)
    : QWidget(parent)
    , m_speed(4)          // 默认速度，与 GameConfig 初始值一致
    , m_noteY(0)
    , m_timerId(0)
    , m_lastHitY(-1) //
    , m_hitAlpha(0)  // 初始完全透明
{
    this->setFocusPolicy(Qt::StrongFocus);
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


    // 绘制残影
    if (m_hitAlpha > 0) {
        // 1. 画一条白色的击打参考线
        painter.setPen(QPen(QColor(255, 255, 255, m_hitAlpha), 2));
        painter.drawLine(20, m_lastHitY, width() - 20, m_lastHitY);

        // 2. 画一个半透明的残影音符
        painter.setBrush(QBrush(QColor(0, 191, 255, m_hitAlpha / 2)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(width()/2, m_lastHitY), 15, 15);
    }

    // 绘制音符（蓝色圆形）
    painter.setBrush(QBrush(QColor(0, 191, 255)));
    painter.setPen(Qt::NoPen);
    int noteRadius = 15;
    painter.drawEllipse(QPointF(width()/2, m_noteY), noteRadius, noteRadius);
}

void PreviewTrack::registerHit()
{
    m_lastHitY = m_noteY; // 当前音符的位置
    m_hitAlpha = 255;     // 触发残影
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

    // 残影淡出动画
    if (m_hitAlpha > 0) {
        m_hitAlpha -= 10; // 每帧减淡
        if (m_hitAlpha < 0) m_hitAlpha = 0;
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

    // ==========================================
    // UI 重构：使用布局管理器与高级 QSS 样式
    // ==========================================

    // 整体水平主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(60, 60, 60, 60); // 留出四周的高级留白
    mainLayout->setSpacing(50);

    // ================= 左侧：控制面板 =================
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(30);

    // 1. 巨大且极简的标题
    QLabel *titleLabel = new QLabel("SETTINGS", this);
    titleLabel->setStyleSheet("color: white; font-size: 50px; font-weight: 900; letter-spacing: 5px;");
    leftLayout->addWidget(titleLabel);

    // 分割线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
    leftLayout->addWidget(line);

    // 通用的高级按钮样式
    QString btnStyle = R"(
        QPushButton {
            background-color: rgba(70, 130, 180, 180);
            border: 2px solid rgba(255, 255, 255, 50);
            border-radius: 10px;
            color: white;
            font-size: 18px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(70, 130, 180, 255);
            border: 2px solid white;
        }
        QPushButton:pressed {
            background-color: rgba(40, 100, 150, 255);
        }
    )";

    // 通用的数值标签样式
    QString valStyle = "background-color: rgba(0, 0, 0, 100); border-radius: 10px; color: #00BFFF; font-size: 24px; font-weight: bold;";

    // 2. 流速调节模块（用一个半透明面板包裹起来）
    QFrame *speedPanel = new QFrame(this);
    speedPanel->setStyleSheet("QFrame { background-color: rgba(255, 255, 255, 10); border-radius: 15px; }");
    QHBoxLayout *speedLayout = new QHBoxLayout(speedPanel);
    speedLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *speedTitle = new QLabel("流速 (Speed)", speedPanel);
    speedTitle->setStyleSheet("color: white; font-size: 18px; background: transparent;");
    m_speedMinusBtn = new QPushButton("-0.05", speedPanel);
    m_speedMinusBtn->setFixedSize(80, 40);
    m_speedMinusBtn->setStyleSheet(btnStyle);

    m_speedLabel = new QLabel(QString::number(GameConfig::instance()->getNoteSpeed(), 'f', 2), speedPanel);
    m_speedLabel->setFixedSize(100, 40);
    m_speedLabel->setAlignment(Qt::AlignCenter);
    m_speedLabel->setStyleSheet(valStyle);

    m_speedPlusBtn = new QPushButton("+0.05", speedPanel);
    m_speedPlusBtn->setFixedSize(80, 40);
    m_speedPlusBtn->setStyleSheet(btnStyle);

    speedLayout->addWidget(speedTitle);
    speedLayout->addStretch();
    speedLayout->addWidget(m_speedMinusBtn);
    speedLayout->addWidget(m_speedLabel);
    speedLayout->addWidget(m_speedPlusBtn);
    leftLayout->addWidget(speedPanel);

    // 3. 偏移调节模块
    QFrame *offsetPanel = new QFrame(this);
    offsetPanel->setStyleSheet("QFrame { background-color: rgba(255, 255, 255, 10); border-radius: 15px; }");
    QHBoxLayout *offsetLayout = new QHBoxLayout(offsetPanel);
    offsetLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *offsetTitle = new QLabel("偏移 (Offset)", offsetPanel);
    offsetTitle->setStyleSheet("color: white; font-size: 18px; background: transparent;");
    m_offsetMinusBtn = new QPushButton("-5 ms", offsetPanel);
    m_offsetMinusBtn->setFixedSize(80, 40);
    m_offsetMinusBtn->setStyleSheet(btnStyle);

    m_offsetLabel = new QLabel(QString::number(GameConfig::instance()->getCurrentOffset()), offsetPanel);
    m_offsetLabel->setFixedSize(100, 40);
    m_offsetLabel->setAlignment(Qt::AlignCenter);
    m_offsetLabel->setStyleSheet(valStyle);

    m_offsetPlusBtn = new QPushButton("+5 ms", offsetPanel);
    m_offsetPlusBtn->setFixedSize(80, 40);
    m_offsetPlusBtn->setStyleSheet(btnStyle);

    offsetLayout->addWidget(offsetTitle);
    offsetLayout->addStretch();
    offsetLayout->addWidget(m_offsetMinusBtn);
    offsetLayout->addWidget(m_offsetLabel);
    offsetLayout->addWidget(m_offsetPlusBtn);
    leftLayout->addWidget(offsetPanel);

    // 把左侧剩余的空间推到最下面
    leftLayout->addStretch();

    // ================= 右侧：预览面板与退出 =================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);

    // 4. 轨道预览
    m_previewTrack = new PreviewTrack(this);
    m_previewTrack->setFixedSize(150, 450); // 稍微拉长一点，更好看
    m_previewTrack->setStyleSheet("background-color: rgba(0, 0, 0, 150); border: 2px solid rgba(255, 255, 255, 50); border-radius: 10px;");
    connect(GameConfig::instance(), &GameConfig::noteSpeedChanged, m_previewTrack, &PreviewTrack::setSpeed);
    m_previewTrack->setSpeed(GameConfig::instance()->getNoteSpeed());

    // 让预览轨道居中靠右
    QHBoxLayout* trackCenterLayout = new QHBoxLayout();
    trackCenterLayout->addStretch();
    trackCenterLayout->addWidget(m_previewTrack);
    rightLayout->addLayout(trackCenterLayout);

    rightLayout->addStretch();

    // 5. 退出按钮
    m_exitBtn = new QPushButton("BACK / 退出", this);
    m_exitBtn->setFixedSize(200, 60);
    m_exitBtn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid #f44336;
            border-radius: 30px;
            color: #f44336;
            font-size: 20px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #f44336;
            color: white;
        }
    )");
    rightLayout->addWidget(m_exitBtn, 0, Qt::AlignRight);

    // 拼装左右两部分 (比例设为 3:2)
    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addLayout(rightLayout, 2);

    // 绑定信号
    connect(m_speedMinusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(-0.05); });
    connect(m_speedPlusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(0.05); });
    connect(m_offsetMinusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(-5); });
    connect(m_offsetPlusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(5); });
    connect(m_exitBtn, &QPushButton::clicked, this, &SettingsWindow::onExitClicked);

    // ---------- 4. 背景音乐 ----------
    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    // TODO: 记得以后把这里改成 "qrc:/" 的资源路径哦！
    m_player->setSource(QUrl::fromLocalFile("setting_bgm.mp3"));
    m_player->setLoops(-1);
    m_player->play();
}

SettingsWindow::~SettingsWindow()
{
    // 停止并释放音乐资源
    m_player->stop();
}

void SettingsWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 【魔法】：纯代码绘制深邃的径向渐变背景，替代丢失的图片！
    QRadialGradient gradient(rect().center(), rect().width() / 1.5);
    gradient.setColorAt(0.0, QColor(45, 55, 72));   // 中心亮一点的深蓝灰
    gradient.setColorAt(1.0, QColor(15, 20, 30));   // 边缘暗下去的极夜黑

    painter.fillRect(rect(), gradient);

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


void SettingsWindow::keyPressEvent(QKeyEvent *event)
{
    // 忽略长按连发
    if (event->isAutoRepeat()) return;

    // 按下了音游的四个按键
    if (event->key() == Qt::Key_D || event->key() == Qt::Key_F ||
        event->key() == Qt::Key_J || event->key() == Qt::Key_K) {

        if (m_previewTrack) {
            m_previewTrack->registerHit(); // 生成残影！
        }
    } else {
        QWidget::keyPressEvent(event); // 其他按键交给父类处理
    }
}

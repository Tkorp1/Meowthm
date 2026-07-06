#include "SettingsWindow.h"
#include "gameconfig.h"          // 全局配置单例
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QDebug>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QAudioOutput>
#include <QMediaPlayer>

// ==================== PreviewTrack 实现 ====================
PreviewTrack::PreviewTrack(QWidget *parent)
    : QWidget(parent)
    , m_speed(4)
    , m_noteY(0)
    , m_timerId(0)
    , m_lastHitY(-1)
    , m_hitAlpha(0)
{
    setFixedSize(160, 450);
    setStyleSheet("background-color: rgba(10, 15, 25, 180); border: 2px solid rgba(0, 191, 255, 80); border-radius: 12px;");

    m_timerId = startTimer(16);

    m_hitSound = new QSoundEffect(this);
    m_hitSound->setSource(QUrl("qrc:/sound/sounds/dong.wav"));
    m_hitSound->setVolume(GameConfig::instance()->getHitSoundVolume() / 100.0f);
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

    // 绘制中心下落轨道线
    painter.setPen(QPen(QColor(255, 255, 255, 50), 2, Qt::DashLine));
    painter.drawLine(width()/2, 0, width()/2, height());

    // 绘制判定线 (科技蓝发光)
    int hitLineY = height() - 50;
    painter.setPen(QPen(QColor(0, 191, 255, 200), 3));
    painter.drawLine(10, hitLineY, width()-10, hitLineY);
    painter.setPen(QPen(QColor(255, 255, 255, 150), 1));
    painter.drawLine(10, hitLineY, width()-10, hitLineY);

    // 击打特效
    if (m_hitAlpha > 0) {
        painter.setPen(QPen(QColor(0, 191, 255, m_hitAlpha), 4));
        painter.drawLine(10, m_lastHitY, width() - 10, m_lastHitY);

        painter.setBrush(QBrush(QColor(0, 191, 255, m_hitAlpha / 3)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(width()/2, m_lastHitY), 25, 25);
    }

    // 绘制下落音符 (蓝底白芯)
    int noteRadius = 16;
    painter.setBrush(QBrush(QColor(0, 191, 255, 200)));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(QPointF(width()/2, m_noteY), noteRadius, noteRadius);
}

void PreviewTrack::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    m_noteY += m_speed * 40;

    if (m_noteY > height() + 100) {
        m_noteY = -(m_speed * 800);
    }

    if (m_hitAlpha > 0) {
        m_hitAlpha -= 12; // 调整消散速度
        if (m_hitAlpha < 0) m_hitAlpha = 0;
    }

    update();
}

void PreviewTrack::setHitSoundVolume(int vol) {
    m_hitSound->setVolume(vol / 100.0f);
}

void PreviewTrack::registerHit()
{
    m_lastHitY = m_noteY;
    m_hitAlpha = 255;
    m_hitSound->play();
}

// ==================== SettingsWindow 实现 ====================
SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(1200, 800);
    setWindowTitle("设置");

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(60, 60, 60, 60);
    mainLayout->setSpacing(50);

    // ================= 左侧：控制面板 =================
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(25);

    // 标题
    QLabel *titleLabel = new QLabel("SYSTEM SETTINGS", this);
    titleLabel->setStyleSheet("color: #00BFFF; font-size: 32px; font-weight: 900; letter-spacing: 5px;");
    leftLayout->addWidget(titleLabel);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: rgba(0, 191, 255, 100);");
    leftLayout->addWidget(line);
    leftLayout->addSpacing(10);


    QString panelStyle = R"(
        QFrame {
            background-color: rgba(20, 25, 35, 180);
            border-left: 5px solid #4682B4;
            border-top: 1px solid rgba(255,255,255,20);
            border-right: 1px solid rgba(255,255,255,20);
            border-bottom: 1px solid rgba(255,255,255,20);
            border-radius: 12px;
        }
        QFrame:hover {
            border-left: 5px solid #00BFFF;
            background-color: rgba(30, 40, 55, 200);
        }
    )";


    QString btnStyle = R"(
        QPushButton {
            background-color: transparent;
            border: 1px solid rgba(0, 191, 255, 80);
            border-radius: 8px;
            color: #00BFFF;
            font-size: 18px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(0, 191, 255, 40);
            border: 1px solid #00BFFF;
            color: white;
        }
        QPushButton:pressed {
            background-color: rgba(0, 191, 255, 100);
        }
    )";

    // 数值
    QString valStyle = "background: transparent; color: white; font-size: 26px; font-weight: bold; border: none;";

    // 滑块
    QString sliderStyle = R"(
        QSlider { background: transparent; }
        QSlider::groove:horizontal {
            border: 1px solid rgba(0, 191, 255, 50);
            height: 6px;
            background: rgba(0, 0, 0, 150);
            border-radius: 3px;
        }
        QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4682B4, stop:1 #00BFFF);
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: white;
            border: 2px solid #00BFFF;
            width: 16px;
            height: 16px;
            margin-top: -6px;
            margin-bottom: -6px;
            border-radius: 8px;
        }
        QSlider::handle:horizontal:hover {
            background: #00BFFF;
            border-color: white;
        }
    )";

    // 2. 流速调节模块
    QFrame *speedPanel = new QFrame(this);
    speedPanel->setStyleSheet(panelStyle);
    QHBoxLayout *speedLayout = new QHBoxLayout(speedPanel);
    speedLayout->setContentsMargins(30, 20, 30, 20);

    QLabel *speedTitle = new QLabel("流速 (SPEED)", speedPanel);
    speedTitle->setStyleSheet("color: rgba(255,255,255,180); font-size: 18px; font-weight: bold; background: transparent; border: none;");

    m_speedMinusBtn = new QPushButton("-0.05", speedPanel);
    m_speedMinusBtn->setFixedSize(75, 40);
    m_speedMinusBtn->setStyleSheet(btnStyle);

    m_speedLabel = new QLabel(QString::number(GameConfig::instance()->getNoteSpeed(), 'f', 2), speedPanel);
    m_speedLabel->setFixedSize(90, 40);
    m_speedLabel->setAlignment(Qt::AlignCenter);
    m_speedLabel->setStyleSheet(valStyle);

    m_speedPlusBtn = new QPushButton("+0.05", speedPanel);
    m_speedPlusBtn->setFixedSize(75, 40);
    m_speedPlusBtn->setStyleSheet(btnStyle);

    speedLayout->addWidget(speedTitle);
    speedLayout->addStretch();
    speedLayout->addWidget(m_speedMinusBtn);
    speedLayout->addWidget(m_speedLabel);
    speedLayout->addWidget(m_speedPlusBtn);
    leftLayout->addWidget(speedPanel);

    // 3. 偏移调节模块
    QFrame *offsetPanel = new QFrame(this);
    offsetPanel->setStyleSheet(panelStyle);
    QHBoxLayout *offsetLayout = new QHBoxLayout(offsetPanel);
    offsetLayout->setContentsMargins(30, 20, 30, 20);

    QLabel *offsetTitle = new QLabel("偏移 (OFFSET)", offsetPanel);
    offsetTitle->setStyleSheet("color: rgba(255,255,255,180); font-size: 18px; font-weight: bold; background: transparent; border: none;");

    m_offsetMinusBtn = new QPushButton("-5", offsetPanel);
    m_offsetMinusBtn->setFixedSize(75, 40);
    m_offsetMinusBtn->setStyleSheet(btnStyle);

    // 在标签里加上 ms 单位，显示效果更好
    m_offsetLabel = new QLabel(QString::number(GameConfig::instance()->getCurrentOffset()) + " ms", offsetPanel);
    m_offsetLabel->setFixedSize(120, 40); // 稍微加宽一点，容纳 ms 字符
    m_offsetLabel->setAlignment(Qt::AlignCenter);
    m_offsetLabel->setStyleSheet(valStyle);

    m_offsetPlusBtn = new QPushButton("+5", offsetPanel);
    m_offsetPlusBtn->setFixedSize(75, 40);
    m_offsetPlusBtn->setStyleSheet(btnStyle);

    offsetLayout->addWidget(offsetTitle);
    offsetLayout->addStretch();
    offsetLayout->addWidget(m_offsetMinusBtn);
    offsetLayout->addWidget(m_offsetLabel);
    offsetLayout->addWidget(m_offsetPlusBtn);
    leftLayout->addWidget(offsetPanel);

    // 5. 打击音量调节模块
    QFrame *volPanel = new QFrame(this);
    volPanel->setStyleSheet(panelStyle);
    QHBoxLayout *volLayout = new QHBoxLayout(volPanel);
    volLayout->setContentsMargins(30, 20, 30, 20);

    QLabel *volTitle = new QLabel("判定音 (SFX VOL)", volPanel);
    volTitle->setStyleSheet("color: rgba(255,255,255,180); font-size: 18px; font-weight: bold; background: transparent; border: none;");

    QPushButton *volMinusBtn = new QPushButton("-5", volPanel);
    volMinusBtn->setFixedSize(75, 40);
    volMinusBtn->setStyleSheet(btnStyle);
    connect(volMinusBtn, &QPushButton::clicked, this, [this](){ onVolButtonClicked(-5); });

    m_volLabel = new QLabel(QString::number(GameConfig::instance()->getHitSoundVolume()), volPanel);
    m_volLabel->setFixedSize(90, 40);
    m_volLabel->setAlignment(Qt::AlignCenter);
    m_volLabel->setStyleSheet(valStyle);

    QPushButton *volPlusBtn = new QPushButton("+5", volPanel);
    volPlusBtn->setFixedSize(75, 40);
    volPlusBtn->setStyleSheet(btnStyle);
    connect(volPlusBtn, &QPushButton::clicked, this, [this](){ onVolButtonClicked(5); });

    volLayout->addWidget(volTitle);
    volLayout->addStretch();
    volLayout->addWidget(volMinusBtn);
    volLayout->addWidget(m_volLabel);
    volLayout->addWidget(volPlusBtn);
    leftLayout->addWidget(volPanel);

    leftLayout->addStretch();

    // ================= 右侧：预览面板与退出 =================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);

    m_exitBtn = new QPushButton("◀ BACK / 返回", this);
    m_exitBtn->setFixedSize(200, 40);
    m_exitBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: rgba(255,255,255,150);
            font-size: 20px;
            font-weight: bold;
            border: none;
            text-align: right;
        }
        QPushButton:hover {
            color: #00BFFF;
        }
    )");
    connect(m_exitBtn, &QPushButton::clicked, this, &SettingsWindow::onExitClicked);
    rightLayout->addWidget(m_exitBtn, 0, Qt::AlignRight);

    rightLayout->addSpacing(60);

    // 预览轨道居中布置
    m_previewTrack = new PreviewTrack(this);
    connect(GameConfig::instance(), &GameConfig::noteSpeedChanged, m_previewTrack, &PreviewTrack::setSpeed);
    m_previewTrack->setSpeed(GameConfig::instance()->getNoteSpeed());

    QHBoxLayout* trackCenterLayout = new QHBoxLayout();
    trackCenterLayout->addStretch();
    trackCenterLayout->addWidget(m_previewTrack);
    rightLayout->addLayout(trackCenterLayout);

    rightLayout->addStretch();


    QLabel *hugeText = new QLabel("SYSTEM\nCFG.", this);
    hugeText->setAlignment(Qt::AlignRight);
    hugeText->setStyleSheet("color: rgba(255, 255, 255, 20); font-size: 80px; font-weight: 900; line-height: 80px; font-family: 'Arial';");
    rightLayout->addWidget(hugeText);

    mainLayout->addLayout(leftLayout, 10);
    mainLayout->addSpacing(80);
    mainLayout->addLayout(rightLayout, 8);

    // 绑定信号
    connect(m_speedMinusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(-0.05); });
    connect(m_speedPlusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(0.05); });
    connect(m_offsetMinusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(-5); });
    connect(m_offsetPlusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(5); });
    //connect(m_offsetSlider, &QSlider::valueChanged, this, &SettingsWindow::onOffsetSliderChanged);

    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl::fromLocalFile("setting_bgm.mp3"));
    m_player->setLoops(-1);
    m_player->play();

    connect(GameConfig::instance(), &GameConfig::hitSoundVolumeChanged,
            m_previewTrack, &PreviewTrack::setHitSoundVolume);
}

SettingsWindow::~SettingsWindow()
{
    m_player->stop();
}


void SettingsWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    painter.fillRect(rect(), QColor(10, 15, 25));

    QPainterPath path;
    path.moveTo(0, height() * 0.4);
    path.lineTo(width() * 0.6, height());
    path.lineTo(0, height());
    path.closeSubpath();

    QLinearGradient shapeGradient(0, height() * 0.4, width() * 0.6, height());
    shapeGradient.setColorAt(0.0, QColor(0, 191, 255, 30));
    shapeGradient.setColorAt(1.0, QColor(138, 43, 226, 10));
    painter.fillPath(path, shapeGradient);


    painter.setPen(QPen(QColor(0, 191, 255, 100), 2));
    painter.drawLine(0, height() * 0.4, width() * 0.6, height());

    QLinearGradient leftGlow(0, 0, 300, 0);
    leftGlow.setColorAt(0.0, QColor(0, 191, 255, 30));
    leftGlow.setColorAt(1.0, QColor(0, 191, 255, 0));
    painter.fillRect(0, 0, 300, height(), leftGlow);

    painter.setPen(QPen(QColor(255, 255, 255, 60), 3));
    int margin = 20;
    int lineLen = 40;

    // 左上角
    painter.drawLine(margin, margin, margin + lineLen, margin);
    painter.drawLine(margin, margin, margin, margin + lineLen);
    // 右上角
    painter.drawLine(width() - margin, margin, width() - margin - lineLen, margin);
    painter.drawLine(width() - margin, margin, width() - margin, margin + lineLen);
    // 左下角
    painter.drawLine(margin, height() - margin, margin + lineLen, height() - margin);
    painter.drawLine(margin, height() - margin, margin, height() - margin - lineLen);
    // 右下角
    painter.drawLine(width() - margin, height() - margin, width() - margin - lineLen, height() - margin);
    painter.drawLine(width() - margin, height() - margin, width() - margin, height() - margin - lineLen);

    QWidget::paintEvent(event);
}

void SettingsWindow::onSpeedButtonClicked(double delta)
{
    double newSpeed = GameConfig::instance()->getNoteSpeed() + delta;
    if (newSpeed < 0.2) newSpeed = 0.2;
    if (newSpeed > 2.0) newSpeed = 2.0;
    GameConfig::instance()->setNoteSpeed(newSpeed);

    updateSpeedLabel(newSpeed);
    this->setFocus();
}

void SettingsWindow::onOffsetButtonClicked(qint64 delta)
{
    qint64 newOffset = GameConfig::instance()->getCurrentOffset() + delta;

    // 限制偏移量范围在 -5000 到 5000 之间
    if (newOffset < -5000) newOffset = -5000;
    if (newOffset > 5000) newOffset = 5000;

    // 直接保存到配置并更新标签
    GameConfig::instance()->setCurrentOffset(newOffset);
    updateOffsetLabel(newOffset);
    this->setFocus();
}

void SettingsWindow::updateOffsetLabel(qint64 offset)
{
    // 保持格式一致，追加 ms 单位
    m_offsetLabel->setText(QString::number(offset) + " ms");
}

void SettingsWindow::updateSpeedLabel(double speed)
{
    m_speedLabel->setText(QString::number(speed, 'f', 2));
}


void SettingsWindow::onExitClicked()
{
    m_player->stop();
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

void SettingsWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_D || event->key() == Qt::Key_F ||
        event->key() == Qt::Key_J || event->key() == Qt::Key_K) {

        if (m_previewTrack) {
            m_previewTrack->registerHit();
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}

void SettingsWindow::onVolButtonClicked(int delta)
{
    int newVol = GameConfig::instance()->getHitSoundVolume() + delta;
    if (newVol < 0) newVol = 0;
    if (newVol > 100) newVol = 100;

    GameConfig::instance()->setHitSoundVolume(newVol);
    updateVolLabel(newVol);
    this->setFocus();
}

void SettingsWindow::updateVolLabel(int vol)
{
    m_volLabel->setText(QString::number(vol));
}

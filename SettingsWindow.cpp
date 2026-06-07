#include "SettingsWindow.h"
#include "gameconfig.h"
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
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
    , m_speed(8) // 基础速度调优
    , m_noteY(-50)
    , m_timerId(0)
    , m_lastHitY(-1)
    , m_hitAlpha(0)
{
    setFixedSize(150, 450); // 加宽一点点，更有轨道的张力
    m_timerId = startTimer(16); // 保持 60FPS

    m_hitSound = new QSoundEffect(this);
    // 动态读取当前配置的音效
    m_hitSound->setSource(QUrl(QString("qrc:/sound/sounds/%1.wav").arg(GameConfig::instance()->getHitSoundType())));
    m_hitSound->setVolume(GameConfig::instance()->getHitSoundVolume() / 100.0f);
}

PreviewTrack::~PreviewTrack()
{
    if (m_timerId) killTimer(m_timerId);
}

void PreviewTrack::setSpeed(double speed)
{
    // 丝滑调校：确保每帧移动的像素量合理，告别掉帧感
    m_speed = speed * 30.0;
}

void PreviewTrack::setHitSoundType(const QString& type)
{
    m_hitSound->setSource(QUrl(QString("qrc:/sound/sounds/%1.wav").arg(type)));
}

void PreviewTrack::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. 轨道全息底光 (上暗下亮)
    QLinearGradient trackBg(0, 0, 0, height());
    trackBg.setColorAt(0.0, QColor(0, 191, 255, 10));
    trackBg.setColorAt(1.0, QColor(0, 191, 255, 60));
    painter.fillRect(rect(), trackBg);

    // 左右发光边框
    painter.setPen(QPen(QColor(0, 191, 255, 150), 2));
    painter.drawLine(0, 0, 0, height());
    painter.drawLine(width(), 0, width(), height());

    // 中线
    painter.setPen(QPen(QColor(255, 255, 255, 50), 2, Qt::DashLine));
    painter.drawLine(width() / 2, 0, width() / 2, height());

    // 2. 判定线 (带有霓虹呼吸感)
    int hitLineY = height() - 50;
    painter.setPen(QPen(QColor(255, 255, 255, 200), 4));
    painter.drawLine(10, hitLineY, width() - 10, hitLineY);

    // 判定线底部光晕
    QLinearGradient hitGlow(0, hitLineY, 0, hitLineY + 30);
    hitGlow.setColorAt(0.0, QColor(255, 255, 255, 100));
    hitGlow.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter.fillRect(0, hitLineY, width(), 30, hitGlow);

    // 3. 打击特效 (水波纹炸开)
    if (m_hitAlpha > 0) {
        painter.setPen(QPen(QColor(0, 255, 255, m_hitAlpha), 3));
        painter.drawLine(0, m_lastHitY, width(), m_lastHitY);

        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(255, 255, 255, m_hitAlpha), 4));
        int expansion = 30 - (m_hitAlpha / 8); // 随透明度扩大
        painter.drawEllipse(QPointF(width() / 2, m_lastHitY), expansion, expansion / 2);
    }

    // 4. 炫酷下落音符 (长椭圆 + 发光核心)
    int noteW = 80;
    int noteH = 20;
    QRectF noteRect((width() - noteW) / 2.0, m_noteY - noteH / 2.0, noteW, noteH);

    painter.setBrush(QColor(0, 255, 255, 220));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRoundedRect(noteRect, noteH / 2, noteH / 2);

    // 音符中心的高亮核心
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(noteRect.adjusted(10, 5, -10, -5), 5, 5);
}

void PreviewTrack::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    m_noteY += m_speed; // 修复：平滑位移，杜绝掉帧

    // 修复：音符离开屏幕后，立即回到轨道正上方，告别漫长等待！
    if (m_noteY > height() + 50) {
        m_noteY = -50;
    }

    if (m_hitAlpha > 0) {
        m_hitAlpha -= 15;
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
    setWindowTitle("SETTINGS");

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(80, 60, 80, 60);
    mainLayout->setSpacing(60);

    // ================= 左侧：高科技控制面板 =================
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(25);

    QLabel *titleLabel = new QLabel("SYSTEM SETUP // 设置", this);
    titleLabel->setStyleSheet("color: #00BFFF; font-size: 42px; font-weight: 900; letter-spacing: 4px;");
    leftLayout->addWidget(titleLabel);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 191, 255, 200), stop:1 rgba(0,0,0,0)); height: 2px;");
    leftLayout->addWidget(line);

    // 全局霓虹按钮与数值框 QSS
    QString btnStyle = R"(
        QPushButton {
            background-color: rgba(0, 191, 255, 20);
            border: 2px solid rgba(0, 191, 255, 80);
            border-radius: 8px;
            color: #00BFFF;
            font-size: 20px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(0, 191, 255, 60);
            border: 2px solid #00BFFF;
            color: white;
        }
        QPushButton:pressed {
            background-color: #00BFFF;
            color: black;
        }
    )";

    QString valStyle = "background-color: rgba(0, 0, 0, 150); border: 1px solid #00BFFF; border-radius: 8px; color: white; font-size: 24px; font-weight: bold;";
    QString panelStyle = "QFrame { background-color: rgba(15, 25, 40, 180); border-left: 4px solid #00BFFF; border-radius: 10px; }";

    QString sliderStyle = R"(
        QSlider::groove:horizontal { border: 1px solid rgba(0, 191, 255, 50); height: 6px; background: rgba(0, 0, 0, 200); border-radius: 3px; }
        QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #005577, stop:1 #00BFFF); border-radius: 3px; }
        QSlider::handle:horizontal { background: white; border: 2px solid #00BFFF; width: 16px; height: 16px; margin-top: -6px; margin-bottom: -6px; border-radius: 8px; }
        QSlider::handle:horizontal:hover { background: #00BFFF; border-color: white; }
    )";

    // [2. 流速调节模块]
    QFrame *speedPanel = new QFrame(this);
    speedPanel->setStyleSheet(panelStyle);
    QHBoxLayout *speedLayout = new QHBoxLayout(speedPanel);
    speedLayout->setContentsMargins(25, 15, 25, 15);

    QLabel *speedTitle = new QLabel("NOTE SPEED // 流速", speedPanel);
    speedTitle->setStyleSheet("color: white; font-size: 18px; font-weight: bold; background: transparent; border: none;");
    m_speedMinusBtn = new QPushButton("◀", speedPanel); m_speedMinusBtn->setFixedSize(60, 40); m_speedMinusBtn->setStyleSheet(btnStyle);
    m_speedLabel = new QLabel(QString::number(GameConfig::instance()->getNoteSpeed(), 'f', 2), speedPanel); m_speedLabel->setFixedSize(100, 40); m_speedLabel->setAlignment(Qt::AlignCenter); m_speedLabel->setStyleSheet(valStyle);
    m_speedPlusBtn = new QPushButton("▶", speedPanel); m_speedPlusBtn->setFixedSize(60, 40); m_speedPlusBtn->setStyleSheet(btnStyle);

    speedLayout->addWidget(speedTitle); speedLayout->addStretch();
    speedLayout->addWidget(m_speedMinusBtn); speedLayout->addWidget(m_speedLabel); speedLayout->addWidget(m_speedPlusBtn);
    leftLayout->addWidget(speedPanel);

    // [3. 偏移调节模块]
    QFrame *offsetPanel = new QFrame(this);
    offsetPanel->setStyleSheet(panelStyle);
    QHBoxLayout *offsetLayout = new QHBoxLayout(offsetPanel);
    offsetLayout->setContentsMargins(25, 15, 25, 15);

    QLabel *offsetTitle = new QLabel("GLOBAL OFFSET // 偏移", offsetPanel);
    offsetTitle->setStyleSheet("color: white; font-size: 18px; font-weight: bold; background: transparent; border: none;");
    m_offsetMinusBtn = new QPushButton("◀", offsetPanel); m_offsetMinusBtn->setFixedSize(60, 40); m_offsetMinusBtn->setStyleSheet(btnStyle);

    m_offsetSlider = new QSlider(Qt::Horizontal, offsetPanel);
    m_offsetSlider->setRange(-500, 500); m_offsetSlider->setValue(GameConfig::instance()->getCurrentOffset()); m_offsetSlider->setFixedWidth(180); m_offsetSlider->setStyleSheet(sliderStyle);

    m_offsetLabel = new QLabel(QString::number(GameConfig::instance()->getCurrentOffset()) + " ms", offsetPanel); m_offsetLabel->setFixedSize(120, 40); m_offsetLabel->setAlignment(Qt::AlignCenter); m_offsetLabel->setStyleSheet(valStyle);
    m_offsetPlusBtn = new QPushButton("▶", offsetPanel); m_offsetPlusBtn->setFixedSize(60, 40); m_offsetPlusBtn->setStyleSheet(btnStyle);

    offsetLayout->addWidget(offsetTitle); offsetLayout->addStretch();
    offsetLayout->addWidget(m_offsetMinusBtn); offsetLayout->addWidget(m_offsetSlider); offsetLayout->addWidget(m_offsetPlusBtn); offsetLayout->addWidget(m_offsetLabel);
    leftLayout->addWidget(offsetPanel);

    // [4. 打击音量调节]
    QFrame *volPanel = new QFrame(this);
    volPanel->setStyleSheet(panelStyle);
    QHBoxLayout *volLayout = new QHBoxLayout(volPanel);
    volLayout->setContentsMargins(25, 15, 25, 15);

    QLabel *volTitle = new QLabel("HIT VOLUME // 音量", volPanel);
    volTitle->setStyleSheet("color: white; font-size: 18px; font-weight: bold; background: transparent; border: none;");
    QPushButton *volMinusBtn = new QPushButton("◀", volPanel); volMinusBtn->setFixedSize(60, 40); volMinusBtn->setStyleSheet(btnStyle);
    connect(volMinusBtn, &QPushButton::clicked, this, [this](){ onVolButtonClicked(-5); });

    m_volLabel = new QLabel(QString::number(GameConfig::instance()->getHitSoundVolume()) + "%", volPanel); m_volLabel->setFixedSize(100, 40); m_volLabel->setAlignment(Qt::AlignCenter); m_volLabel->setStyleSheet(valStyle);

    QPushButton *volPlusBtn = new QPushButton("▶", volPanel); volPlusBtn->setFixedSize(60, 40); volPlusBtn->setStyleSheet(btnStyle);
    connect(volPlusBtn, &QPushButton::clicked, this, [this](){ onVolButtonClicked(5); });

    volLayout->addWidget(volTitle); volLayout->addStretch();
    volLayout->addWidget(volMinusBtn); volLayout->addWidget(m_volLabel); volLayout->addWidget(volPlusBtn);
    leftLayout->addWidget(volPanel);

    // [5. 新增：判定音效切换 (DONG / MEOW)]
    QFrame *soundTypePanel = new QFrame(this);
    soundTypePanel->setStyleSheet(panelStyle);
    QHBoxLayout *soundTypeLayout = new QHBoxLayout(soundTypePanel);
    soundTypeLayout->setContentsMargins(25, 15, 25, 15);

    QLabel *soundTypeTitle = new QLabel("SOUND STYLE // 音效", soundTypePanel);
    soundTypeTitle->setStyleSheet("color: white; font-size: 18px; font-weight: bold; background: transparent; border: none;");

    QPushButton *m_soundToggleBtn = new QPushButton(this);
    m_soundToggleBtn->setFixedSize(220, 40);
    // 专门给它加点紫色元素，暗示猫娘特效
    m_soundToggleBtn->setStyleSheet(btnStyle + "QPushButton { border-color: #DA70D6; color: #DA70D6; } QPushButton:hover { background-color: rgba(218, 112, 214, 60); border-color: #FF00FF; color: white; }");

    QString currentSound = GameConfig::instance()->getHitSoundType();
    m_soundToggleBtn->setText(currentSound == "meow" ? "STYLE: MEOW (喵)" : "STYLE: DONG (咚)");

    connect(m_soundToggleBtn, &QPushButton::clicked, this, [this, m_soundToggleBtn]() {
        QString current = GameConfig::instance()->getHitSoundType();
        QString next = (current == "dong") ? "meow" : "dong";
        GameConfig::instance()->setHitSoundType(next);
        m_soundToggleBtn->setText(next == "meow" ? "STYLE: MEOW (喵)" : "STYLE: DONG (咚)");
        this->setFocus();
    });

    soundTypeLayout->addWidget(soundTypeTitle); soundTypeLayout->addStretch();
    soundTypeLayout->addWidget(m_soundToggleBtn);
    leftLayout->addWidget(soundTypePanel);

    leftLayout->addStretch();

    // ================= 右侧：全息预览舱与退出 =================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QLabel *previewTitle = new QLabel("CALIBRATION TRACK\n[ 按 D, F, J, K 测试 ]", this);
    previewTitle->setAlignment(Qt::AlignCenter);
    previewTitle->setStyleSheet("color: rgba(255,255,255,100); font-size: 14px; letter-spacing: 2px;");
    rightLayout->addWidget(previewTitle);
    rightLayout->addSpacing(10);

    m_previewTrack = new PreviewTrack(this);
    m_previewTrack->setStyleSheet("background-color: transparent;"); // 样式已经全在 paintEvent 里画了
    connect(GameConfig::instance(), &GameConfig::noteSpeedChanged, m_previewTrack, &PreviewTrack::setSpeed);
    m_previewTrack->setSpeed(GameConfig::instance()->getNoteSpeed());

    QHBoxLayout* trackCenterLayout = new QHBoxLayout();
    trackCenterLayout->addStretch();
    trackCenterLayout->addWidget(m_previewTrack);
    trackCenterLayout->addStretch();
    rightLayout->addLayout(trackCenterLayout);

    rightLayout->addStretch();

    m_exitBtn = new QPushButton("APPLY & RETURN", this);
    m_exitBtn->setFixedSize(250, 60);
    m_exitBtn->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255, 69, 0, 20);
            border: 2px solid #FF4500;
            border-radius: 8px;
            color: #FF4500;
            font-size: 20px;
            font-weight: 900;
            letter-spacing: 2px;
        }
        QPushButton:hover {
            background-color: #FF4500;
            color: white;
            border: 2px solid white;
        }
    )");
    rightLayout->addWidget(m_exitBtn, 0, Qt::AlignCenter);

    mainLayout->addLayout(leftLayout, 5);
    mainLayout->addLayout(rightLayout, 4);

    // 绑定信号
    connect(m_speedMinusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(-0.05); });
    connect(m_speedPlusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(0.05); });
    connect(m_offsetMinusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(-5); });
    connect(m_offsetPlusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(5); });
    connect(m_offsetSlider, &QSlider::valueChanged, this, &SettingsWindow::onOffsetSliderChanged);
    connect(m_exitBtn, &QPushButton::clicked, this, &SettingsWindow::onExitClicked);

    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl::fromLocalFile("setting_bgm.mp3"));
    m_player->setLoops(-1);
    m_player->play();

    connect(GameConfig::instance(), &GameConfig::hitSoundVolumeChanged, m_previewTrack, &PreviewTrack::setHitSoundVolume);
    connect(GameConfig::instance(), &GameConfig::hitSoundTypeChanged, m_previewTrack, &PreviewTrack::setHitSoundType);
}

SettingsWindow::~SettingsWindow()
{
    m_player->stop();
}

void SettingsWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制深邃的赛博空间背景
    QRadialGradient gradient(rect().center(), rect().width() / 1.2);
    gradient.setColorAt(0.0, QColor(15, 25, 45)); // 中心偏深蓝
    gradient.setColorAt(1.0, QColor(5, 10, 15));  // 边缘纯黑死寂
    painter.fillRect(rect(), gradient);

    // 点缀一些底部的霓虹反光
    QLinearGradient bottomGlow(0, height() - 100, 0, height());
    bottomGlow.setColorAt(0.0, QColor(0, 191, 255, 0));
    bottomGlow.setColorAt(1.0, QColor(0, 191, 255, 30));
    painter.fillRect(0, height() - 100, width(), 100, bottomGlow);

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
    if (newOffset < -500) newOffset = -500;
    if (newOffset > 500) newOffset = 500;

    m_offsetSlider->setValue(newOffset);
    this->setFocus();
}

void SettingsWindow::onOffsetSliderChanged(int value)
{
    GameConfig::instance()->setCurrentOffset(value);
    updateOffsetLabel(value);
    this->setFocus();
}

void SettingsWindow::updateSpeedLabel(double speed) { m_speedLabel->setText(QString::number(speed, 'f', 2)); }
void SettingsWindow::updateOffsetLabel(qint64 offset) { m_offsetLabel->setText(QString::number(offset) + " ms"); }
void SettingsWindow::updateVolLabel(int vol) { m_volLabel->setText(QString::number(vol) + "%"); }

void SettingsWindow::onExitClicked()
{
    m_player->stop();
    this->close();
    if (parentWidget()) parentWidget()->show();
}

void SettingsWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_D || event->key() == Qt::Key_F ||
        event->key() == Qt::Key_J || event->key() == Qt::Key_K) {
        if (m_previewTrack) m_previewTrack->registerHit();
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

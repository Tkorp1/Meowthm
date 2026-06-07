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
    , m_speed(4)
    , m_noteY(0)
    , m_timerId(0)
    , m_lastHitY(-1)
    , m_hitAlpha(0)
{
    setFixedSize(100, 300);
    setStyleSheet("background-color: rgba(0,0,0,100); border: 1px solid white;");

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
    m_speed = speed * 3;
}

void PreviewTrack::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(width()/2, 0, width()/2, height());

    int hitLineY = height() - 30;
    painter.setPen(QPen(Qt::yellow, 3));
    painter.drawLine(10, hitLineY, width()-10, hitLineY);

    if (m_hitAlpha > 0) {
        painter.setPen(QPen(QColor(255, 255, 255, m_hitAlpha), 2));
        painter.drawLine(20, m_lastHitY, width() - 20, m_lastHitY);

        painter.setBrush(QBrush(QColor(0, 191, 255, m_hitAlpha / 2)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(width()/2, m_lastHitY), 15, 15);
    }

    painter.setBrush(QBrush(QColor(0, 191, 255)));
    painter.setPen(Qt::NoPen);
    int noteRadius = 15;
    painter.drawEllipse(QPointF(width()/2, m_noteY), noteRadius, noteRadius);
}

void PreviewTrack::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    m_noteY += m_speed * 16;

    if (m_noteY > height() + 100) {
        m_noteY = -(m_speed * 800);
    }

    if (m_hitAlpha > 0) {
        m_hitAlpha -= 10;
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
    leftLayout->setSpacing(30);

    QLabel *titleLabel = new QLabel("SETTINGS", this);
    titleLabel->setStyleSheet("color: white; font-size: 50px; font-weight: 900; letter-spacing: 5px;");
    leftLayout->addWidget(titleLabel);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
    leftLayout->addWidget(line);

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

    QString valStyle = "background-color: rgba(0, 0, 0, 100); border-radius: 10px; color: #00BFFF; font-size: 24px; font-weight: bold;";

    // 滑块专属赛博朋克QSS样式（高画质无损发光感）
    QString sliderStyle = R"(
        QSlider::groove:horizontal {
            border: 1px solid rgba(255, 255, 255, 30);
            height: 8px;
            background: rgba(0, 0, 0, 150);
            border-radius: 4px;
        }
        QSlider::sub-page:horizontal {
            background: #00BFFF;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: white;
            border: 2px solid #00BFFF;
            width: 18px;
            height: 18px;
            margin-top: -5px;
            margin-bottom: -5px;
            border-radius: 9px;
        }
        QSlider::handle:horizontal:hover {
            background: #00BFFF;
            border-color: white;
        }
    )";

    // 2. 流速调节模块
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

    // 3. 偏移调节模块（已整合高集成度布局控制）
    QFrame *offsetPanel = new QFrame(this);
    offsetPanel->setStyleSheet("QFrame { background-color: rgba(255, 255, 255, 10); border-radius: 15px; }");
    QHBoxLayout *offsetLayout = new QHBoxLayout(offsetPanel);
    offsetLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *offsetTitle = new QLabel("偏移 (Offset)", offsetPanel);
    offsetTitle->setStyleSheet("color: white; font-size: 18px; background: transparent;");

    m_offsetMinusBtn = new QPushButton("-5 ms", offsetPanel);
    m_offsetMinusBtn->setFixedSize(80, 40);
    m_offsetMinusBtn->setStyleSheet(btnStyle);

    // 初始化并注入高级动态滑块
    m_offsetSlider = new QSlider(Qt::Horizontal, offsetPanel);
    m_offsetSlider->setRange(-500, 500);
    m_offsetSlider->setValue(GameConfig::instance()->getCurrentOffset());
    m_offsetSlider->setFixedWidth(200); // 设定契合左侧比例的滑块宽度
    m_offsetSlider->setStyleSheet(sliderStyle);

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
    offsetLayout->addWidget(m_offsetSlider); // 滑块优雅插在控频组件中
    offsetLayout->addWidget(m_offsetPlusBtn);
    offsetLayout->addWidget(m_offsetLabel);
    leftLayout->addWidget(offsetPanel);

    // 5. 打击音量调节模块
    QFrame *volPanel = new QFrame(this);
    volPanel->setStyleSheet("QFrame { background-color: rgba(255, 255, 255, 10); border-radius: 15px; }");
    QHBoxLayout *volLayout = new QHBoxLayout(volPanel);
    volLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *volTitle = new QLabel("判定音量 (Hit Sound)", volPanel);
    volTitle->setStyleSheet("color: white; font-size: 18px; background: transparent;");
    QPushButton *volMinusBtn = new QPushButton("-5", volPanel);
    volMinusBtn->setFixedSize(80, 40);
    volMinusBtn->setStyleSheet(btnStyle);
    connect(volMinusBtn, &QPushButton::clicked, this, [this](){ onVolButtonClicked(-5); });

    m_volLabel = new QLabel(QString::number(GameConfig::instance()->getHitSoundVolume()), volPanel);
    m_volLabel->setFixedSize(100, 40);
    m_volLabel->setAlignment(Qt::AlignCenter);
    m_volLabel->setStyleSheet(valStyle);

    QPushButton *volPlusBtn = new QPushButton("+5", volPanel);
    volPlusBtn->setFixedSize(80, 40);
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

    m_previewTrack = new PreviewTrack(this);
    m_previewTrack->setFixedSize(150, 450);
    m_previewTrack->setStyleSheet("background-color: rgba(0, 0, 0, 150); border: 2px solid rgba(255, 255, 255, 50); border-radius: 10px;");
    connect(GameConfig::instance(), &GameConfig::noteSpeedChanged, m_previewTrack, &PreviewTrack::setSpeed);
    m_previewTrack->setSpeed(GameConfig::instance()->getNoteSpeed());

    QHBoxLayout* trackCenterLayout = new QHBoxLayout();
    trackCenterLayout->addStretch();
    trackCenterLayout->addWidget(m_previewTrack);
    rightLayout->addLayout(trackCenterLayout);

    rightLayout->addStretch();

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

    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addLayout(rightLayout, 2);

    // 绑定信号
    connect(m_speedMinusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(-0.05); });
    connect(m_speedPlusBtn, &QPushButton::clicked, this, [this](){ onSpeedButtonClicked(0.05); });
    connect(m_offsetMinusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(-5); });
    connect(m_offsetPlusBtn, &QPushButton::clicked, this, [this](){ onOffsetButtonClicked(5); });
    connect(m_offsetSlider, &QSlider::valueChanged, this, &SettingsWindow::onOffsetSliderChanged); // 滑块连接
    connect(m_exitBtn, &QPushButton::clicked, this, &SettingsWindow::onExitClicked);

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

    QRadialGradient gradient(rect().center(), rect().width() / 1.5);
    gradient.setColorAt(0.0, QColor(45, 55, 72));
    gradient.setColorAt(1.0, QColor(15, 20, 30));

    painter.fillRect(rect(), gradient);

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

// 偏移值按钮调节槽函数（现在同步推动滑块）
void SettingsWindow::onOffsetButtonClicked(qint64 delta)
{
    qint64 newOffset = GameConfig::instance()->getCurrentOffset() + delta;
    if (newOffset < -500) newOffset = -500;
    if (newOffset > 500) newOffset = 500;

    // 直接操作滑块更新，滑块会触发 valueChanged 信号连带修改配置及文本
    m_offsetSlider->setValue(newOffset);

    this->setFocus();
}

// 核心处理：滑块拖拽事件响应槽函数
void SettingsWindow::onOffsetSliderChanged(int value)
{
    GameConfig::instance()->setCurrentOffset(value);
    updateOffsetLabel(value);

    this->setFocus(); // 确保鼠标释放后按键焦点回到主窗口，键盘捕获不失效
}

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



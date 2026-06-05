#include "ProfileWindow.h"
#include "gameconfig.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QFrame>

ProfileWindow::ProfileWindow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(1200, 800);
    setWindowTitle("PLAYER TERMINAL");

    // 全局主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(60, 60, 60, 60);
    mainLayout->setSpacing(40);

    // 通用面板样式
    QString panelStyle = "QFrame { background-color: rgba(15, 20, 30, 200); border: 2px solid rgba(0, 191, 255, 100); border-radius: 15px; }";
    QString titleStyle = "color: #00BFFF; font-size: 24px; font-weight: 900; letter-spacing: 2px; background: transparent; border: none;";
    QString btnStyle = R"(
        QPushButton {
            background-color: transparent; border: 2px solid #00BFFF; border-radius: 8px; color: #00BFFF; font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background-color: #00BFFF; color: black; }
    )";

    // ==========================================
    // 左侧面板：PERSONAL DATA (个人数据区) - 保持不变
    // ==========================================
    QFrame *leftPanel = new QFrame(this);
    leftPanel->setStyleSheet(panelStyle);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(30, 30, 30, 30);
    leftLayout->setSpacing(20);

    QLabel *leftTitle = new QLabel("PERSONAL DATA // 档案", leftPanel);
    leftTitle->setStyleSheet(titleStyle);
    leftLayout->addWidget(leftTitle);

    // 1. 玩家头像
    m_avatarLabel = new QLabel("MEOW", leftPanel);
    m_avatarLabel->setFixedSize(120, 120);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setStyleSheet("background-color: rgba(0, 191, 255, 30); border: 3px solid #00BFFF; border-radius: 60px; color: white; font-size: 24px; font-weight: bold;");

    QHBoxLayout *avatarLayout = new QHBoxLayout();
    avatarLayout->addStretch();
    avatarLayout->addWidget(m_avatarLabel);
    avatarLayout->addStretch();
    leftLayout->addLayout(avatarLayout);

    // 2. 昵称模块
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QString savedName = GameConfig::instance()->getCurrentPlayer();
    if (savedName.isEmpty()) savedName = "Unknown Pilot";

    m_nicknameDisplay = new QLabel(savedName, leftPanel);
    m_nicknameDisplay->setStyleSheet("color: white; font-size: 32px; font-weight: bold; background: transparent; border: none;");

    m_nicknameEditBtn = new QPushButton("EDIT", leftPanel);
    m_nicknameEditBtn->setFixedSize(60, 30);
    m_nicknameEditBtn->setStyleSheet(btnStyle);

    m_nicknameEdit = new QLineEdit(leftPanel);
    m_nicknameEdit->setText(savedName);
    m_nicknameEdit->setStyleSheet("background: rgba(0,0,0,150); color: white; font-size: 24px; border: 1px solid #00BFFF; border-radius: 5px;");
    m_nicknameEdit->hide();

    m_nicknameSaveBtn = new QPushButton("SAVE", leftPanel);
    m_nicknameSaveBtn->setFixedSize(60, 30);
    m_nicknameSaveBtn->setStyleSheet("QPushButton { background-color: #00BFFF; color: black; font-weight: bold; border-radius: 5px; }");
    m_nicknameSaveBtn->hide();

    nameLayout->addStretch();
    nameLayout->addWidget(m_nicknameDisplay);
    nameLayout->addWidget(m_nicknameEdit);
    nameLayout->addWidget(m_nicknameEditBtn);
    nameLayout->addWidget(m_nicknameSaveBtn);
    nameLayout->addStretch();
    leftLayout->addLayout(nameLayout);

    // 3. 简介模块
    QLabel *bioHeader = new QLabel("BIOGRAPHY / 简介：", leftPanel);
    bioHeader->setStyleSheet("color: #A9A9A9; font-size: 16px; background: transparent; border: none;");
    leftLayout->addWidget(bioHeader);

    m_bioDisplay = new QLabel("这个人很神秘，还没有留下神经连接记录...", leftPanel);
    m_bioDisplay->setWordWrap(true);
    m_bioDisplay->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_bioDisplay->setStyleSheet("color: white; font-size: 18px; background: rgba(0,0,0,100); border-radius: 5px; padding: 10px; border: none;");
    m_bioDisplay->setMinimumHeight(100);

    m_bioEdit = new QTextEdit(leftPanel);
    m_bioEdit->setText(m_bioDisplay->text());
    m_bioEdit->setStyleSheet("background: rgba(0,0,0,150); color: white; font-size: 18px; border: 1px solid #00BFFF; border-radius: 5px;");
    m_bioEdit->hide();

    leftLayout->addWidget(m_bioDisplay);
    leftLayout->addWidget(m_bioEdit);

    QHBoxLayout *bioBtnLayout = new QHBoxLayout();
    m_bioEditBtn = new QPushButton("EDIT BIO", leftPanel);
    m_bioEditBtn->setFixedSize(100, 30);
    m_bioEditBtn->setStyleSheet(btnStyle);

    m_bioSaveBtn = new QPushButton("SAVE BIO", leftPanel);
    m_bioSaveBtn->setFixedSize(100, 30);
    m_bioSaveBtn->setStyleSheet("QPushButton { background-color: #00BFFF; color: black; font-weight: bold; border-radius: 5px; }");
    m_bioSaveBtn->hide();

    bioBtnLayout->addStretch();
    bioBtnLayout->addWidget(m_bioEditBtn);
    bioBtnLayout->addWidget(m_bioSaveBtn);
    leftLayout->addLayout(bioBtnLayout);

    leftLayout->addStretch();

    // ==========================================
    // 右侧面板：COMBAT STATS (游戏数据区)
    // ==========================================
    QFrame *rightPanel = new QFrame(this);
    rightPanel->setStyleSheet(panelStyle);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(30, 30, 30, 30);
    rightLayout->setSpacing(20);

    QLabel *rightTitle = new QLabel("COMBAT RECORDS // 作战记录", rightPanel);
    rightTitle->setStyleSheet(titleStyle);
    rightLayout->addWidget(rightTitle);

    // 高级数据排版 (使用 HTML 表格实现两端对齐)
    QString statsHtml =
        "<table width='100%' style='font-size: 20px; line-height: 2.5;'>"
        "<tr><td style='color:#A9A9A9;'>SYSTEM RATING (系统评级)</td><td align='right' style='color:#FFD700; font-weight:bold; font-size: 28px;'>S+</td></tr>"
        "<tr><td style='color:#A9A9A9;'>TOTAL PLAY TIME (总连接时长)</td><td align='right' style='color:white; font-weight:bold;'>12h 45m</td></tr>"
        "<tr><td style='color:#A9A9A9;'>TRACKS CLEARED (完成曲目)</td><td align='right' style='color:white; font-weight:bold;'>34</td></tr>"
        "<tr><td style='color:#A9A9A9;'>FULL COMBOS (全连击次数)</td><td align='right' style='color:#00FFFF; font-weight:bold;'>8</td></tr>"
        "<tr><td style='color:#A9A9A9;'>AVERAGE ACCURACY (平均精准度)</td><td align='right' style='color:#32CD32; font-weight:bold;'>96.84%</td></tr>"
        "<tr><td style='color:#A9A9A9;'>PEAK KPS (最高爆发手速)</td><td align='right' style='color:#FF4500; font-weight:bold;'>14.2</td></tr>"
        "</table>";

    QLabel *statsInfo = new QLabel(statsHtml, rightPanel);
    statsInfo->setStyleSheet("background: transparent; border: none;");
    rightLayout->addWidget(statsInfo);

    // 增加一条霓虹分割线
    QFrame *statsLine = new QFrame(rightPanel);
    statsLine->setFrameShape(QFrame::HLine);
    statsLine->setStyleSheet("background-color: rgba(0, 191, 255, 100); border: none;");
    rightLayout->addWidget(statsLine);

    QLabel *footerQuote = new QLabel("\"Rhythm is the soul's neural link.\"", rightPanel);
    footerQuote->setStyleSheet("color: rgba(255,255,255,100); font-style: italic; font-size: 16px; background: transparent; border: none;");
    footerQuote->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(footerQuote);

    rightLayout->addStretch();

    // ==========================================
    // 底部：退出按钮
    // ==========================================
    QPushButton *backBtn = new QPushButton("◀ RETURN", this);
    backBtn->setFixedSize(150, 40);
    backBtn->setStyleSheet("QPushButton { background: transparent; color: rgba(255,255,255,150); font-size: 18px; font-weight: bold; border: none; } QPushButton:hover { color: white; }");

    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 1);

    backBtn->setParent(this);
    backBtn->move(40, 30);

    // 信号槽连接
    connect(m_nicknameEditBtn, &QPushButton::clicked, this, &ProfileWindow::onEditNickname);
    connect(m_nicknameSaveBtn, &QPushButton::clicked, this, &ProfileWindow::onSaveNickname);
    connect(m_bioEditBtn, &QPushButton::clicked, this, &ProfileWindow::onEditBio);
    connect(m_bioSaveBtn, &QPushButton::clicked, this, &ProfileWindow::onSaveBio);

    connect(backBtn, &QPushButton::clicked, this, [this](){
        this->close();
        if (parentWidget()) parentWidget()->show();
    });
}

ProfileWindow::~ProfileWindow() {}

void ProfileWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRadialGradient gradient(rect().center(), rect().width() / 1.2);
    gradient.setColorAt(0.0, QColor(25, 35, 55));
    gradient.setColorAt(1.0, QColor(10, 15, 25));
    painter.fillRect(rect(), gradient);
}

void ProfileWindow::onEditNickname()
{
    m_nicknameDisplay->hide();
    m_nicknameEditBtn->hide();
    m_nicknameEdit->setText(m_nicknameDisplay->text());
    m_nicknameEdit->show();
    m_nicknameSaveBtn->show();
    m_nicknameEdit->setFocus();
}

void ProfileWindow::onSaveNickname()
{
    QString text = m_nicknameEdit->text().trimmed();
    if (text.isEmpty()) text = "Unknown Pilot";

    GameConfig::instance()->setCurrentPlayer(text);
    m_nicknameDisplay->setText(text);

    m_nicknameEdit->hide();
    m_nicknameSaveBtn->hide();
    m_nicknameDisplay->show();
    m_nicknameEditBtn->show();
}

void ProfileWindow::onEditBio()
{
    m_bioDisplay->hide();
    m_bioEditBtn->hide();
    m_bioEdit->setText(m_bioDisplay->text());
    m_bioEdit->show();
    m_bioSaveBtn->show();
    m_bioEdit->setFocus();
}

void ProfileWindow::onSaveBio()
{
    QString text = m_bioEdit->toPlainText().trimmed();
    if (text.isEmpty()) text = "这个人很神秘，还没有留下打歌记录...";

    m_bioDisplay->setText(text);

    m_bioEdit->hide();
    m_bioSaveBtn->hide();
    m_bioDisplay->show();
    m_bioEditBtn->show();
}

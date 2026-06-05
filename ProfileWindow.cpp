#include "ProfileWindow.h"
#include "gameconfig.h"
#include "loginwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QFrame>
#include <QMessageBox>

ProfileWindow::ProfileWindow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(1200, 800);
    setWindowTitle("PLAYER TERMINAL");

    // 全局主布局 (留出四周高大上的留白)
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(60, 60, 60, 60);
    mainLayout->setSpacing(40);

    // 通用面板样式 (深色磨砂玻璃 + 霓虹蓝边框)
    QString panelStyle = "QFrame { background-color: rgba(15, 20, 30, 200); border: 2px solid rgba(0, 191, 255, 100); border-radius: 15px; }";
    QString titleStyle = "color: #00BFFF; font-size: 24px; font-weight: 900; letter-spacing: 2px; background: transparent; border: none;";
    QString btnStyle = R"(
        QPushButton {
            background-color: transparent; border: 2px solid #00BFFF; border-radius: 8px; color: #00BFFF; font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background-color: #00BFFF; color: black; }
    )";

    // ==========================================
    // 左侧面板：PERSONAL DATA (个人数据区)
    // ==========================================
    QFrame *leftPanel = new QFrame(this);
    leftPanel->setStyleSheet(panelStyle);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(30, 30, 30, 30);
    leftLayout->setSpacing(20);

    QLabel *leftTitle = new QLabel("PERSONAL DATA // 档案", leftPanel);
    leftTitle->setStyleSheet(titleStyle);
    leftLayout->addWidget(leftTitle);

    // 1. 玩家头像 (用一个发光的圆圈包裹文字代替)
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

    leftLayout->addStretch(); // 把上面的东西往上顶

    // ==========================================
    // 右侧面板：ACCOUNT SYSTEM (通行证与数据)
    // ==========================================
    QFrame *rightPanel = new QFrame(this);
    rightPanel->setStyleSheet(panelStyle);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(30, 30, 30, 30);
    rightLayout->setSpacing(20);

    QLabel *rightTitle = new QLabel("MEOWTHM PASS // 游戏通行证", rightPanel);
    rightTitle->setStyleSheet(titleStyle);
    rightLayout->addWidget(rightTitle);

    // 账号信息占位区
    QString accHtml =
        "<div style='font-size: 18px; line-height: 1.5;'>"
        "<span style='color:#A9A9A9;'>ACCOUNT STATUS:</span> <span style='color:#FF4500; font-weight:bold;'>GUEST (未登录)</span><br><br>"
        "<span style='color:#A9A9A9;'>USER ID:</span> <span style='color:white;'>[ ENCRYPTED ]</span><br><br>"
        "<span style='color:#A9A9A9;'>LINKED EMAIL:</span> <span style='color:white;'>-- NULL --</span><br><br>"
        "</div>";
    QLabel *accInfo = new QLabel(accHtml, rightPanel);
    accInfo->setStyleSheet("background: transparent; border: none;");
    rightLayout->addWidget(accInfo);

    // 预留的登录/注册按钮
    QPushButton *loginBtn = new QPushButton("LOGIN / REGISTER SYSTEM", rightPanel);
    loginBtn->setFixedSize(300, 50);
    loginBtn->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255, 69, 0, 50); border: 2px solid #FF4500; border-radius: 25px; color: #FF4500; font-size: 16px; font-weight: bold; letter-spacing: 2px;
        }
        QPushButton:hover { background-color: #FF4500; color: white; border: 2px solid white; }
    )");
    rightLayout->addWidget(loginBtn, 0, Qt::AlignCenter);

    rightLayout->addSpacing(30);

    // 游戏数据占位区
    QLabel *statsTitle = new QLabel("COMBAT RECORDS // 作战记录", rightPanel);
    statsTitle->setStyleSheet("color: #DDA0DD; font-size: 20px; font-weight: 900; background: transparent; border: none;");
    rightLayout->addWidget(statsTitle);

    QString statsHtml =
        "<div style='font-size: 18px; line-height: 1.5;'>"
        "<span style='color:#A9A9A9;'>TOTAL PLAY TIME:</span> <span style='color:white;'>04h 12m</span><br>"
        "<span style='color:#A9A9A9;'>TRACKS CLEARED:</span> <span style='color:white;'>12</span><br>"
        "<span style='color:#A9A9A9;'>AVERAGE ACCURACY:</span> <span style='color:white;'>94.20%</span><br>"
        "</div>";
    QLabel *statsInfo = new QLabel(statsHtml, rightPanel);
    statsInfo->setStyleSheet("background: transparent; border: none;");
    rightLayout->addWidget(statsInfo);

    rightLayout->addStretch();

    // ==========================================
    // 底部：退出按钮
    // ==========================================
    QPushButton *backBtn = new QPushButton("◀ RETURN", this);
    backBtn->setFixedSize(150, 40);
    backBtn->setStyleSheet("QPushButton { background: transparent; color: rgba(255,255,255,150); font-size: 18px; font-weight: bold; border: none; } QPushButton:hover { color: white; }");

    // 组装主布局 (左右比例 1:1)
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 1);

    // 把退出按钮放在绝对位置的右上角
    backBtn->setParent(this);
    backBtn->move(40, 30);

    // ==========================================
    // 信号槽连接
    // ==========================================
    connect(m_nicknameEditBtn, &QPushButton::clicked, this, &ProfileWindow::onEditNickname);
    connect(m_nicknameSaveBtn, &QPushButton::clicked, this, &ProfileWindow::onSaveNickname);
    connect(m_bioEditBtn, &QPushButton::clicked, this, &ProfileWindow::onEditBio);
    connect(m_bioSaveBtn, &QPushButton::clicked, this, &ProfileWindow::onSaveBio);
    connect(loginBtn, &QPushButton::clicked, this, &ProfileWindow::onLoginClicked);

    connect(backBtn, &QPushButton::clicked, this, [this](){
        this->close();
        if (parentWidget()) parentWidget()->show();
    });
}

ProfileWindow::~ProfileWindow() {}

// ==========================================
// 背景绘制
// ==========================================
void ProfileWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 使用深邃的径向渐变背景，抛弃低质量图片
    QRadialGradient gradient(rect().center(), rect().width() / 1.2);
    gradient.setColorAt(0.0, QColor(25, 35, 55));   // 中心蓝灰
    gradient.setColorAt(1.0, QColor(10, 15, 25));   // 边缘极夜黑
    painter.fillRect(rect(), gradient);
}

// ==========================================
// 交互逻辑
// ==========================================
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

void ProfileWindow::onLoginClicked()
{
    /// 创建我们炫酷的登录窗口！
    LoginWindow *loginWin = new LoginWindow(this);

    // 让弹窗自动跑到整个界面的正中央
    int dx = (this->width() - loginWin->width()) / 2;
    int dy = (this->height() - loginWin->height()) / 2;
    loginWin->move(dx, dy);

    // 强制显示在最顶层
    loginWin->raise();
    loginWin->show();
}

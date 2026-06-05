#include "loginwindow.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
{
    // 作为一个独立的小弹窗，不需要全屏
    setFixedSize(500, 400);

    // 去掉系统自带的丑陋白底边框，设为无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    // ==========================================
    // UI 布局
    // ==========================================
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    // 1. 标题
    QLabel *titleLabel = new QLabel("MEOWTHM SYSTEM AUTH", this);
    titleLabel->setStyleSheet("color: #00BFFF; font-size: 24px; font-weight: 900; letter-spacing: 2px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 分割线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: rgba(0, 191, 255, 100);");
    mainLayout->addWidget(line);

    mainLayout->addSpacing(10);

    // 2. 账号输入框
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("请输入 UID...");
    m_usernameEdit->setFixedHeight(45);
    m_usernameEdit->setStyleSheet("QLineEdit { background: rgba(0, 0, 0, 150); border: 1px solid #00BFFF; border-radius: 5px; color: white; font-size: 16px; padding-left: 15px; } QLineEdit:focus { border: 2px solid white; }");
    mainLayout->addWidget(m_usernameEdit);

    // 3. 密码输入框
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("请输入密钥...");
    m_passwordEdit->setFixedHeight(45);
    m_passwordEdit->setEchoMode(QLineEdit::Password); // 【魔法】变成密码圆点！
    m_passwordEdit->setStyleSheet(m_usernameEdit->styleSheet());
    mainLayout->addWidget(m_passwordEdit);

    mainLayout->addSpacing(20);

    // 4. 按钮区
    QHBoxLayout *btnLayout = new QHBoxLayout();

    m_loginBtn = new QPushButton("LOGIN / 授权登入", this);
    m_loginBtn->setFixedHeight(45);
    m_loginBtn->setStyleSheet("QPushButton { background-color: rgba(0, 191, 255, 50); border: 2px solid #00BFFF; border-radius: 5px; color: #00BFFF; font-size: 16px; font-weight: bold; } QPushButton:hover { background-color: #00BFFF; color: black; }");

    m_registerBtn = new QPushButton("REGISTER / 注册", this);
    m_registerBtn->setFixedHeight(45);
    m_registerBtn->setStyleSheet("QPushButton { background-color: transparent; border: 2px solid #A9A9A9; border-radius: 5px; color: #A9A9A9; font-size: 16px; font-weight: bold; } QPushButton:hover { background-color: white; color: black; border: 2px solid white; }");

    btnLayout->addWidget(m_loginBtn);
    btnLayout->addWidget(m_registerBtn);
    mainLayout->addLayout(btnLayout);

    // 5. 底部关闭按钮
    m_closeBtn = new QPushButton("CANCEL", this);
    m_closeBtn->setStyleSheet("background: transparent; color: rgba(255, 255, 255, 100); border: none; font-size: 14px; text-decoration: underline;");
    mainLayout->addWidget(m_closeBtn, 0, Qt::AlignCenter);

    // ==========================================
    // 信号槽连接
    // ==========================================
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &LoginWindow::close);

    // 【新增】：炫酷淡入动画
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(opacityEffect);
    QPropertyAnimation *anim = new QPropertyAnimation(opacityEffect, "opacity");
    anim->setDuration(200);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

LoginWindow::~LoginWindow() {}

void LoginWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 画一个带圆角和描边的暗色半透明面板
    painter.setBrush(QColor(15, 20, 30, 230)); // 极夜黑，略微透明
    painter.setPen(QPen(QColor(0, 191, 255, 150), 2)); // 霓虹蓝描边
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 15, 15);
}

// 按钮点击事件（暂时写死提示框，下一步我们接真实数据库）
void LoginWindow::onLoginClicked()
{
    QString user = m_usernameEdit->text().trimmed();
    QString pwd = m_passwordEdit->text().trimmed();

    if(user.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "ERROR", "账号或密码不能为空！");
        return;
    }

    QMessageBox::information(this, "SYSTEM", "后端连接中...这是下一节课的内容！");
}

void LoginWindow::onRegisterClicked()
{
    QMessageBox::information(this, "SYSTEM", "准备在本地新建玩家档案...这也是下一节课的内容！");
}

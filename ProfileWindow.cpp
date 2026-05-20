#include "ProfileWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

ProfileWindow::ProfileWindow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(1200, 800);
    setWindowTitle("个人档案");

    // ---------- 昵称模块 ----------
    // 触发按钮（透明，覆盖背景图上的图标区域）
    m_nicknameTriggerBtn = new QPushButton("", this);
    m_nicknameTriggerBtn->setGeometry(355, 390, 50, 50);
    m_nicknameTriggerBtn->setStyleSheet("background-color: transparent;");
    m_nicknameTriggerBtn->setCursor(Qt::PointingHandCursor);

    // 昵称输入框（初始隐藏）
    m_nicknameEdit = new QLineEdit(this);
    m_nicknameEdit->setPlaceholderText("输入新昵称");
    m_nicknameEdit->setStyleSheet("background-color: rgba(255,255,255,200); color:black;font-size: 16px; padding: 5px;");
    m_nicknameEdit->setGeometry(400, 50, 300, 40);
    m_nicknameEdit->hide();

    // 昵称保存按钮（初始隐藏）
    m_nicknameSaveBtn = new QPushButton("保存", this);
    m_nicknameSaveBtn->setStyleSheet("background-color: #5F9EA0; color: white; font-size: 14px; border-radius: 3px;");
    m_nicknameSaveBtn->setGeometry(720, 50, 80, 40);
    m_nicknameSaveBtn->hide();

    // 昵称显示标签
    m_nicknameDisplayLabel = new QLabel(this);
    m_nicknameDisplayLabel->setAlignment(Qt::AlignCenter);
    m_nicknameDisplayLabel->setStyleSheet(
        "background-color: transparent; "
        "color: #4B0082; "
        "font-family: 'Yuanti SC', 'STYuanti', 'Apple SD Gothic Neo', 'Comic Neue', 'Comic Sans MS'; "
        "font-size: 24px; "
        "padding: 18px;"
        );
    m_nicknameDisplayLabel->setGeometry(105, 374, 240, 80);
    m_nicknameDisplayLabel->setText("小喵");

    // ---------- 简介模块（多行文本） ----------
    // 触发按钮
    m_bioTriggerBtn = new QPushButton("", this);
    m_bioTriggerBtn->setGeometry(120, 475, 40, 40);
    m_bioTriggerBtn->setStyleSheet("background-color: transparent; color: white; font-size: 14px; border-radius: 5px;");
    m_bioTriggerBtn->setCursor(Qt::PointingHandCursor);

    // 多行输入框（初始隐藏）
    m_bioEdit = new QTextEdit(this);
    m_bioEdit->setPlaceholderText("");
    m_bioEdit->setStyleSheet("background-color:transparent; color:black;font-size: 16px; padding: 5px;");
    m_bioEdit->setGeometry(50, 510, 400, 130);
    m_bioEdit->hide();

    // 简介保存按钮（初始隐藏）
    m_bioSaveBtn = new QPushButton("保存简介", this);
    m_bioSaveBtn->setStyleSheet("background-color: #5F9EA0; color: white; font-size: 14px; border-radius: 3px;");
    m_bioSaveBtn->setGeometry(250, 680, 100, 40);
    m_bioSaveBtn->hide();

    // 简介显示标签（初始显示已有内容）
    m_bioDisplayLabel = new QLabel(this);
    m_bioDisplayLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_bioDisplayLabel->setWordWrap(true);  // 自动换行
    m_bioDisplayLabel->setStyleSheet(
        "background-color: transparent; "
        "color: black; "
        "font-size: 16px; "
        "padding: 10px; "
        "border-radius: 5px;"
        );
    m_bioDisplayLabel->setGeometry(45, 500, 400, 130);
    m_bioDisplayLabel->setText("喵呜～");

    // ---------- 返回主界面按钮 ----------
    m_backBtn = new QPushButton("", this);
    m_backBtn->setGeometry(1050, 700, 120, 70);
    m_backBtn->setStyleSheet("background-color: transparent; color: white; font-size: 16px; border-radius: 8px;");
    m_backBtn->setCursor(Qt::PointingHandCursor);

    // 信号槽连接
    connect(m_nicknameTriggerBtn, &QPushButton::clicked, this, &ProfileWindow::onNicknameTriggerClicked);
    connect(m_nicknameSaveBtn, &QPushButton::clicked, this, &ProfileWindow::onNicknameSaveClicked);
    connect(m_bioTriggerBtn, &QPushButton::clicked, this, &ProfileWindow::onBioTriggerClicked);
    connect(m_bioSaveBtn, &QPushButton::clicked, this, &ProfileWindow::onBioSaveClicked);
    connect(m_backBtn, &QPushButton::clicked, this, &ProfileWindow::onBackClicked);
}

ProfileWindow::~ProfileWindow() {}

void ProfileWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg("bg2.png");
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), QColor(30, 30, 40));
    }
    QWidget::paintEvent(event);
}

// 昵称：显示编辑界面
void ProfileWindow::onNicknameTriggerClicked()
{
    m_nicknameEdit->show();
    m_nicknameSaveBtn->show();
    m_nicknameDisplayLabel->hide();
    m_nicknameEdit->setFocus();
    m_nicknameEdit->clear();
}

// 昵称：保存
void ProfileWindow::onNicknameSaveClicked()
{
    QString text = m_nicknameEdit->text().trimmed();
    if (text.isEmpty()) {
        m_nicknameDisplayLabel->setText("昵称不能为空！");
        m_nicknameDisplayLabel->show();
        return;
    }
    m_nicknameDisplayLabel->setText(text);
    m_nicknameDisplayLabel->show();
    m_nicknameEdit->hide();
    m_nicknameSaveBtn->hide();
}

// 简介：显示多行编辑框和保存按钮
void ProfileWindow::onBioTriggerClicked()
{
    m_bioEdit->show();
    m_bioSaveBtn->show();
    m_bioDisplayLabel->hide();
    m_bioEdit->setFocus();
    m_bioEdit->clear();
}

// 简介：保存多行内容
void ProfileWindow::onBioSaveClicked()
{
    QString text = m_bioEdit->toPlainText().trimmed();
    if (text.isEmpty()) {
        m_bioDisplayLabel->setText("（未填写）");
    } else {
        m_bioDisplayLabel->setText(text);
    }
    m_bioDisplayLabel->show();
    m_bioEdit->hide();
    m_bioSaveBtn->hide();
}

// 返回主界面
void ProfileWindow::onBackClicked()
{
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

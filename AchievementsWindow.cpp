#include "AchievementsWindow.h"
#include "gameconfig.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QPainter>
#include <QFrame>

// 定义我们要展示的成就数据结构
struct AchData {
    int id;
    QString title;
    QString desc;
};

AchievementsWindow::AchievementsWindow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(1200, 800);
    setWindowTitle("ACHIEVEMENTS SYSTEM");

    // 全局主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 40, 50, 40);
    mainLayout->setSpacing(20);

    // ==========================================
    // 顶部标题和返回按钮
    // ==========================================
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QPushButton *backBtn = new QPushButton("◀ RETURN", this);
    backBtn->setFixedSize(120, 40);
    backBtn->setStyleSheet("QPushButton { background: transparent; color: rgba(255,255,255,150); font-size: 18px; font-weight: bold; border: none; text-align: left; } QPushButton:hover { color: white; }");
    connect(backBtn, &QPushButton::clicked, this, [this](){
        this->close();
        if (parentWidget()) parentWidget()->show();
    });

    QLabel *titleLabel = new QLabel("ACHIEVEMENTS // 荣誉记录", this);
    titleLabel->setStyleSheet("color: #FFD700; font-size: 32px; font-weight: 900; letter-spacing: 4px;");
    titleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    headerLayout->addWidget(backBtn);
    headerLayout->addWidget(titleLabel);
    mainLayout->addLayout(headerLayout);

    // 霓虹分割线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: rgba(255, 215, 0, 100);");
    mainLayout->addWidget(line);

    // ==========================================
    // 核心滚动区域：成就列表
    // ==========================================
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *listWidget = new QWidget();
    listWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *listLayout = new QVBoxLayout(listWidget);
    listLayout->setSpacing(20);
    listLayout->setContentsMargins(10, 10, 10, 10);

    // 录入你策划的所有成就！
    QList<AchData> allAchievements = {
        {1, "滚木", ""},
        {2, "打一首歌", "有手就行"},
        {3, "菜就多练", "菜，就多练；输不起，就别玩～"},
        {4, "我承认你有点鸟不起", "tr(我) < tr(您)"},
        {5, "大神啊", "！？强强？！"},
        {6, "令 人 忍 俊 不 禁", "沉默微笑做不到（我猜是因为他没玩过Meowthm）"}
        // 以后有互动成就，直接往这里加！
    };

    // 动态生成成就卡片
    for (const AchData& ach : allAchievements) {
        bool isUnlocked = GameConfig::instance()->isAchievementUnlocked(ach.id);

        QFrame *card = new QFrame(listWidget);
        card->setFixedHeight(120);
        QHBoxLayout *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(20, 20, 30, 20);

        // 左侧图标 (无论是否解锁，都显示奖杯，只是颜色不同)
        QLabel *iconLabel = new QLabel(card);
        iconLabel->setFixedSize(80, 80);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setText("🏆"); // 【修改】：去掉锁头，统一使用奖杯

        // 中间文字区
        QVBoxLayout *textLayout = new QVBoxLayout();

        // 加上 background: transparent
        QLabel *title = new QLabel(ach.title, card);
        title->setStyleSheet(isUnlocked ? "color: white; font-size: 24px; font-weight: bold; background: transparent;"
                                        : "color: rgba(255,255,255,100); font-size: 24px; font-weight: bold; background: transparent;");

        QLabel *desc = new QLabel(ach.desc, card);
        desc->setStyleSheet(isUnlocked ? "color: #A9A9A9; font-size: 16px; background: transparent;"
                                       : "color: rgba(255,255,255,50); font-size: 16px; background: transparent;");

        textLayout->addWidget(title);
        textLayout->addWidget(desc);
        textLayout->addStretch();

        // 右侧状态
        QLabel *statusLabel = new QLabel(isUnlocked ? "UNLOCKED" : "LOCKED", card);
        statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        // 根据是否解锁，赋予不同的高级样式！
        if (isUnlocked) {
            // 解锁状态：发散金光
            card->setStyleSheet("QFrame { background-color: rgba(255, 215, 0, 20); border: none; border-radius: 15px; }");
            iconLabel->setStyleSheet("background-color: rgba(255, 215, 0, 50); border: 2px solid #FFD700; border-radius: 10px; color: #FFD700; font-size: 32px; font-weight: bold;");
            statusLabel->setStyleSheet("color: #FFD700; font-size: 20px; font-weight: 900; letter-spacing: 2px; border: none; background: transparent;");
        } else {
            // 未解锁状态
            card->setStyleSheet("QFrame { background-color: rgba(255, 255, 255, 5); border: none; border-radius: 15px; }");
            iconLabel->setStyleSheet("background-color: rgba(0, 0, 0, 100); border: 2px solid rgba(255, 255, 255, 30); border-radius: 10px; color: rgba(255,255,255,50); font-size: 32px; font-weight: bold;");
            statusLabel->setStyleSheet("color: rgba(255, 255, 255, 50); font-size: 20px; font-weight: bold; letter-spacing: 2px; border: none; background: transparent;");
        }

        cardLayout->addWidget(iconLabel);
        cardLayout->addSpacing(20);
        cardLayout->addLayout(textLayout);
        cardLayout->addStretch();
        cardLayout->addWidget(statusLabel);

        listLayout->addWidget(card);
    }

    listLayout->addStretch();
    scrollArea->setWidget(listWidget);
    mainLayout->addWidget(scrollArea);
}

AchievementsWindow::~AchievementsWindow() {}

void AchievementsWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 科技感深渊黑金渐变背景
    QRadialGradient gradient(rect().center(), rect().width() / 1.2);
    gradient.setColorAt(0.0, QColor(30, 25, 10)); // 中心微微泛金
    gradient.setColorAt(1.0, QColor(10, 10, 15)); // 边缘极夜黑
    painter.fillRect(rect(), gradient);
}

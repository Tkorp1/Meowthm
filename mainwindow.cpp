#include "MainWindow.h"
#include "SelectSongWindow.h"
#include "SettingsWindow.h"
#include "AchievementsWindow.h"
#include "PokeWindow.h"
#include "ProfileWindow.h"
#include <QBrush>
#include <QFont>
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QDebug>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QKeyEvent>
#include <QCursor>
#include <QPointF>
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    // 基本初始化
    : QMainWindow(parent)
    , ui(nullptr)
    , view(new QGraphicsView(this))
    , scene(new QGraphicsScene(this))
{
    setWindowTitle("游戏初始界面");
    setFixedSize(1200, 800);

    // 图形视图框架初始化
    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(view);

    // 添加背景图（保持比例，不拉伸）
    QPixmap bgPixmap("bg.png");
    if (!bgPixmap.isNull()) {
        QPixmap scaledBg = bgPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QGraphicsPixmapItem *bgItem = new QGraphicsPixmapItem(scaledBg);
        bgItem->setPos((width() - scaledBg.width()) / 2, (height() - scaledBg.height()) / 2);
        bgItem->setZValue(-1);
        scene->addItem(bgItem);
    } else {

    }

    // 按钮样式：完全透明，无文字，无边框，实现点击背景图相应区域即可触发
    QString buttonStyle = R"(
        QPushButton {
            background-color: transparent;
            color: transparent;
            border: none;
        }
        QPushButton:hover {
            background-color: transparent;
        }
        QPushButton:pressed {
            background-color: transparent;
        }
    )";
    // 按钮大小
    int btnWidth = 200;
    int btnHeight = 110;
    // 按钮相关参数
    struct ButtonData {
        QString text;
        int x;
        int y;
        std::function<void()> onClick;
    };

    QList<ButtonData> buttons = {
        { "退出游戏", 1025, 24, [this]() { onExitGame(); } },
        { "进入选曲界面", 115, 498, [this]() { onSelectSong(); } },
        { "设置", 361, 498, [this]() { onSettings(); } },
        { "个人档案", 625, 498, [this]() { onProfile(); } },
        { "成就", 880, 498, [this]() { onAchievements(); } },
        { "戳", 727, 336, [this]() { onPoke(); } }
    };
    // 一键初始化所有按钮
    for (const auto &data : buttons) {
        QPushButton *btn = new QPushButton(data.text);
        btn->setFixedSize(btnWidth, btnHeight);
        btn->setStyleSheet(buttonStyle);
        QGraphicsProxyWidget *proxy = scene->addWidget(btn);
        proxy->setPos(data.x, data.y);
        QObject::connect(btn, &QPushButton::clicked, data.onClick);
    }

    view->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
// 连接所有按钮到槽函数
void MainWindow::onSelectSong()
{
    SelectSongWindow *window = new SelectSongWindow(this);
    window->show();
    //this->hide();
}

void MainWindow::onSettings()
{
    SettingsWindow *window = new SettingsWindow(this);
    window->show();
    //this->hide();
}

void MainWindow::onAchievements()
{
    AchievementsWindow *window = new AchievementsWindow(this);
    window->show();
    //this->hide();
}

void MainWindow::onPoke()
{
    PokeWindow *window = new PokeWindow(this);
    window->show();
    //this->hide();
}

void MainWindow::onExitGame()
{
    this->close();
}

void MainWindow::onProfile()
{
    ProfileWindow *window = new ProfileWindow(this);
    window->show();
    //this->hide();
}

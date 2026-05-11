#include "MainWindow.h"
#include "ButtonItem.h"
#include "SelectSongWindow.h"    // 选曲窗口
#include "SettingsWindow.h"      // 设置窗口
#include "AchievementsWindow.h"  // 成就窗口
#include "PokeWindow.h"          // 戳窗口
#include <QBrush>
#include <QFont>
#include <QGraphicsTextItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)
    , view(new QGraphicsView(this))
    , scene(new QGraphicsScene(this))
{
    setWindowTitle("游戏初始界面");
    setFixedSize(800, 600);

    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(view);

    scene->setBackgroundBrush(QBrush(QColor(30, 30, 40)));

    // 标题
    QGraphicsTextItem *title = scene->addText("Meowthm");
    QFont titleFont("Microsoft YaHei", 28, QFont::Bold);
    title->setFont(titleFont);
    title->setDefaultTextColor(Qt::white);
    qreal titleWidth = title->boundingRect().width();
    title->setPos((width() - titleWidth) / 2, 80);

    // 为了方便布局，定义按钮的垂直起始位置和间距
    int startY = 220;
    int stepY = 80;
    int currentY = startY;

    // 选曲按钮
    ButtonItem *enterBtn = new ButtonItem(
        "btn_enter_normal.png",
        "btn_enter_hover.png",
        "btn_enter_pressed.png"
        );
    qreal btnWidth = enterBtn->pixmap().width();
    enterBtn->setPos((width() - btnWidth) / 2, currentY);
    scene->addItem(enterBtn);
    connect(enterBtn, &ButtonItem::clicked, this, &MainWindow::onSelectSong);
    currentY += stepY;

    // 设置按钮
    ButtonItem *settingsBtn = new ButtonItem(
        "btn_settings_normal.png",
        "btn_settings_hover.png",
        "btn_settings_pressed.png"
        );
    settingsBtn->setPos((width() - btnWidth) / 2, currentY);
    scene->addItem(settingsBtn);
    connect(settingsBtn, &ButtonItem::clicked, this, &MainWindow::onSettings);
    currentY += stepY;

    // 成就按钮
    ButtonItem *achievementsBtn = new ButtonItem(
        "btn_achievements_normal.png",
        "btn_achievements_hover.png",
        "btn_achievements_pressed.png"
        );
    achievementsBtn->setPos((width() - btnWidth) / 2, currentY);
    scene->addItem(achievementsBtn);
    connect(achievementsBtn, &ButtonItem::clicked, this, &MainWindow::onAchievements);
    currentY += stepY;

    // 戳按钮
    ButtonItem *pokeBtn = new ButtonItem(
        "btn_poke_normal.png",
        "btn_poke_hover.png",
        "btn_poke_pressed.png"
        );
    pokeBtn->setPos((width() - btnWidth) / 2, currentY);
    scene->addItem(pokeBtn);
    connect(pokeBtn, &ButtonItem::clicked, this, &MainWindow::onPoke);
    currentY += stepY;

    // 退出按钮
    ButtonItem *exitBtn = new ButtonItem(
        "btn_exit_normal.png",
        "btn_exit_hover.png",
        "btn_exit_pressed.png"
        );
    exitBtn->setPos((width() - btnWidth) / 2, currentY);
    scene->addItem(exitBtn);
    connect(exitBtn, &ButtonItem::clicked, this, &MainWindow::onExitGame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSelectSong()
{
    SelectSongWindow *window = new SelectSongWindow(this);
    window->show();
    this->hide();
}

void MainWindow::onSettings()
{
    SettingsWindow *window = new SettingsWindow(this);
    window->show();
    this->hide();
}

void MainWindow::onAchievements()
{
    AchievementsWindow *window = new AchievementsWindow(this);
    window->show();
    this->hide();
}

void MainWindow::onPoke()
{
    PokeWindow *window = new PokeWindow(this);
    window->show();
    this->hide();
}

void MainWindow::onExitGame()
{
    this->close();
}

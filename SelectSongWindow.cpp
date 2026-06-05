#include "SelectSongWindow.h"
#include "SettingsWindow.h"
#include "PokeWindow.h"
#include "ProfileWindow.h"
#include "AchievementsWindow.h"
#include "MainWindow.h"
#include "gamescene.h"
#include "gameconfig.h"

#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>

SelectSongWindow::SelectSongWindow(QWidget *parent)
    : QWidget(parent)
{

    setWindowTitle("MUSIC SELECT");
    setFixedSize(1200, 800);

    // ==========================================
    // 1. 全局主布局 (左右分栏)
    // ==========================================
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(40);

    // ==========================================
    // 2. 左侧：高冷流线型歌曲列表
    // ==========================================
    QWidget *leftArea = new QWidget(this);
    leftArea->setFixedWidth(550);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftArea);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // 左侧大标题
    QLabel *listTitle = new QLabel("TRACKS", leftArea);
    listTitle->setStyleSheet("color: #00BFFF; font-size: 32px; font-weight: 900; letter-spacing: 5px;");
    leftLayout->addWidget(listTitle);

    QFrame *line = new QFrame(leftArea);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: rgba(0, 191, 255, 100);");
    leftLayout->addWidget(line);

    // 歌曲网格容器
    QWidget *gridContainer = new QWidget();
    gridContainer->setStyleSheet("background: transparent;");
    QGridLayout *grid = new QGridLayout(gridContainer);
    grid->setSpacing(20);
    grid->setContentsMargins(0, 10, 10, 10);

    QList<SongInfo> songs = GameConfig::instance()->getSongs();
    if (songs.isEmpty()) {
        QLabel *noSongLabel = new QLabel("NO TRACKS FOUND", gridContainer);
        noSongLabel->setStyleSheet("color: rgba(255,255,255,100); font-size: 24px; font-weight: bold;");
        grid->addWidget(noSongLabel, 0, 0);
    } else {
        int row = 0, col = 0;
        for (int i = 0; i < songs.size(); ++i) {
            const SongInfo &song = songs[i];

            // 歌曲卡片
            QPushButton *card = new QPushButton();
            card->setFixedSize(250, 120); // 宽卡片
            card->setObjectName(QString("card_%1").arg(i));


            QPixmap cover(song.coverPath);
            QString bgStyle;


            // TODO: 这里目前有点问题，有图片可能也无法显示
            if (!cover.isNull()) {
                // 1. 如果文件夹里有 cover.jpg，就把图片作为背景居中显示
                bgStyle = QString("background-image: url(%1); background-position: center;").arg(song.coverPath);
            } else {
                // 2. 如果没封面，使用统一的纯色背景
                bgStyle = "background-color: rgba(30, 40, 55, 200);";
            }

            // 使用 QString::arg() 把生成的背景动态注入到 QSS 里
            card->setStyleSheet(QString(R"(
                QPushButton {
                    %1
                    border-left: 5px solid #4682B4;
                    border-top: 1px solid rgba(255,255,255,30);
                    border-right: 1px solid rgba(255,255,255,30);
                    border-bottom: 1px solid rgba(255,255,255,30);
                    color: white;
                    font-size: 18px;
                    font-weight: bold;
                    text-align: left;
                    padding-left: 20px;
                }
                QPushButton:hover {
                    border-left: 5px solid #00BFFF;
                    /* 如果是纯色背景，悬停时会微微变亮；如果是图片，则蓝色边框高亮 */
                    background-color: rgba(60, 80, 100, 250);
                }
            )").arg(bgStyle));

            card->setText(song.name);
            m_cardButtons.append(card);
            connect(card, &QPushButton::clicked, this, &SelectSongWindow::onSongCardClicked);

            grid->addWidget(card, row, col);
            col++;
            if (col >= 2) { col = 0; row++; }
        }
    }

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    leftLayout->addWidget(scrollArea);

    // ==========================================
    // 3. 右侧：纯文字排版 + 极简按钮区
    // ==========================================
    QWidget *rightArea = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(40, 0, 0, 0);
    rightLayout->setAlignment(Qt::AlignTop | Qt::AlignRight);

    // 返回主菜单按钮 (纯文字)
    QPushButton *backBtn = new QPushButton("◀ BACK", rightArea);
    backBtn->setFixedSize(120, 40);
    backBtn->setStyleSheet("QPushButton { background: transparent; color: rgba(255,255,255,150); font-size: 18px; font-weight: bold; border: none; text-align: right; } QPushButton:hover { color: white; }");
    connect(backBtn, &QPushButton::clicked, this, &SelectSongWindow::onBackToMain);
    rightLayout->addWidget(backBtn, 0, Qt::AlignRight);

    rightLayout->addSpacing(80); // 留白是高级感的来源

    // 超大背景水印排版 (替代了那些花里胡哨的图片)
    QLabel *hugeText = new QLabel("MEOWTHM\nSYSTEM", rightArea);
    hugeText->setAlignment(Qt::AlignRight);
    hugeText->setStyleSheet("color: rgba(255, 255, 255, 10); font-size: 80px; font-weight: 900; line-height: 80px;");
    rightLayout->addWidget(hugeText);

    rightLayout->addStretch();

    // 纯代码绘制的高级菜单按钮
    QString menuBtnStyle = R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid rgba(255,255,255,50);
            border-radius: 25px;
            color: white;
            font-size: 16px;
            font-weight: bold;
            letter-spacing: 2px;
        }
        QPushButton:hover {
            background-color: white;
            color: black;
            border: 2px solid white;
        }
    )";

    QPushButton *pokeBtn = new QPushButton("INTERACT / 互动", rightArea);
    pokeBtn->setFixedSize(250, 50);
    pokeBtn->setStyleSheet(menuBtnStyle);
    connect(pokeBtn, &QPushButton::clicked, this, &SelectSongWindow::onPoke);
    rightLayout->addWidget(pokeBtn, 0, Qt::AlignRight);
    rightLayout->addSpacing(15);

    QPushButton *profileBtn = new QPushButton("PROFILE / 档案", rightArea);
    profileBtn->setFixedSize(250, 50);
    profileBtn->setStyleSheet(menuBtnStyle);
    connect(profileBtn, &QPushButton::clicked, this, &SelectSongWindow::onProfile);
    rightLayout->addWidget(profileBtn, 0, Qt::AlignRight);
    rightLayout->addSpacing(15);

    QPushButton *achieveBtn = new QPushButton("ACHIEVEMENTS / 成就", rightArea);
    achieveBtn->setFixedSize(250, 50);
    achieveBtn->setStyleSheet(menuBtnStyle);
    connect(achieveBtn, &QPushButton::clicked, this, &SelectSongWindow::onAchievements);
    rightLayout->addWidget(achieveBtn, 0, Qt::AlignRight);
    rightLayout->addSpacing(15);

    QPushButton *settingsBtn = new QPushButton("SETTINGS / 设置", rightArea);
    settingsBtn->setFixedSize(250, 50);
    settingsBtn->setStyleSheet(menuBtnStyle);
    connect(settingsBtn, &QPushButton::clicked, this, &SelectSongWindow::onSettings);
    rightLayout->addWidget(settingsBtn, 0, Qt::AlignRight);

    // 将左右区域加入主布局
    mainLayout->addWidget(leftArea);
    mainLayout->addWidget(rightArea);
}

SelectSongWindow::~SelectSongWindow() {}

// ==========================================
// 纯代码绘制深邃的赛博朋克极简背景
// ==========================================
void SelectSongWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制深色斜向渐变背景，完全代替 bg_select.png
    QLinearGradient bgGradient(0, 0, width(), height());
    bgGradient.setColorAt(0.0, QColor(15, 20, 30));   // 左上极夜黑
    bgGradient.setColorAt(0.5, QColor(25, 35, 55));   // 中间深灰蓝
    bgGradient.setColorAt(1.0, QColor(10, 15, 25));   // 右下深渊黑
    painter.fillRect(rect(), bgGradient);

    // 在右侧画几个倾斜的巨大半透明几何光带，增加设计感
    QPainterPath path;
    path.moveTo(width() * 0.6, 0);
    path.lineTo(width() * 0.9, 0);
    path.lineTo(width(), height() * 0.5);
    path.lineTo(width(), height());
    path.lineTo(width() * 0.5, height());
    path.closeSubpath();

    QLinearGradient shapeGradient(width() * 0.5, 0, width(), height());
    shapeGradient.setColorAt(0.0, QColor(0, 191, 255, 15)); // 微微发光的霓虹蓝
    shapeGradient.setColorAt(1.0, QColor(255, 255, 255, 5));
    painter.fillPath(path, shapeGradient);

    QWidget::paintEvent(event);
}

// ==========================================
// 槽函数部分保持不变
// ==========================================
void SelectSongWindow::onSongCardClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int idx = m_cardButtons.indexOf(btn);
    if (idx < 0) return;

    QList<SongInfo> songs = GameConfig::instance()->getSongs();
    if (idx >= songs.size()) return;

    const SongInfo &song = songs[idx];
    GameScene *game = new GameScene(song.mapFolderPath);
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    this->hide();
}

void SelectSongWindow::onSettings() { SettingsWindow *window = new SettingsWindow(this); window->show(); }
void SelectSongWindow::onPoke() { PokeWindow *window = new PokeWindow(this); window->show(); }
void SelectSongWindow::onProfile() { ProfileWindow *window = new ProfileWindow(this); window->show(); }
void SelectSongWindow::onAchievements() { AchievementsWindow *window = new AchievementsWindow(this); window->show(); }
void SelectSongWindow::onBackToMain() { this->close(); if (parentWidget()) parentWidget()->show(); }

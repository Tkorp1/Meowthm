#include "SelectSongWindow.h"
#include "SettingsWindow.h"
#include "PokeWindow.h"
#include "ProfileWindow.h"
#include "AchievementsWindow.h"
#include "MainWindow.h"
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <functional>

SelectSongWindow::SelectSongWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("选曲界面");
    setFixedSize(1200, 800);

<<<<<<< HEAD
    // 按钮样式：完全透明，无文字，无边框
    QString buttonStyle = R"(
=======
    // ========== 左侧：歌曲网格区域（使用布局，不改为绝对定位） ==========
    QWidget *leftArea = new QWidget(this);
    leftArea->setGeometry(20, 100, 500, 600);  // 占据大部分区域，留出右侧和底部空间
    QVBoxLayout *leftLayout = new QVBoxLayout(leftArea);

    // 歌曲网格容器
    QWidget *gridContainer = new QWidget();
    gridContainer->setStyleSheet("background: transparent;");
    QGridLayout *grid = new QGridLayout(gridContainer);
    grid->setSpacing(25);
    grid->setContentsMargins(10, 10, 10, 10);

    QList<SongInfo> songs = GameConfig::instance()->getSongs();
    if (songs.isEmpty()) {
        QLabel *noSongLabel = new QLabel("没有找到歌曲，请检查 maps 目录", gridContainer);
        noSongLabel->setStyleSheet("color: white; font-size: 20px;");
        grid->addWidget(noSongLabel, 0, 0);
    } else {
        const int cols = 2; // 每行4个卡片
        int row = 0, col = 0;
        for (int i = 0; i < songs.size(); ++i) {
            const SongInfo &song = songs[i];

            QPushButton *card = new QPushButton();
            card->setFixedSize(180, 240);
            card->setObjectName(QString("card_%1").arg(i));
            card->setStyleSheet(R"(
                QPushButton {
                    background-color: transparent;
                    border: 2px solid #aaa;
                    border-radius: 0px;
                    color: white;
                    font-size: 14px;
                    font-weight: bold;
                    text-align: center;
                    padding-top: 160px;
                }
                QPushButton:hover {
                    background-color: rgba(255,255,255,150);
                    border-color: #ffcc00;
                    color: black;
                }
            )");

            // 加载封面
            QPixmap cover(song.coverPath);
            if (!cover.isNull()) {
                QPixmap scaled = cover.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                card->setIcon(QIcon(scaled));
                card->setIconSize(QSize(160, 160));
            } else {
                // 没有封面时显示默认图标或占位符
                card->setText(song.name);

            }

            // 如果已经有封面文字，则不再重复显示；否则只显示歌曲名
            if (card->icon().isNull()) {
                card->setText(song.name);
            } else {
                card->setText(song.name);
            }

            // 保存卡片以便索引
            m_cardButtons.append(card);
            connect(card, &QPushButton::clicked, this, &SelectSongWindow::onSongCardClicked);

            grid->addWidget(card, row, col);
            col++;
            if (col >= cols) {
                col = 0;
                row++;
            }
        }
    }

    // 将网格放入滚动区域
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    leftLayout->addWidget(scrollArea);
    leftArea->setLayout(leftLayout);

    // ========== 右侧：功能按钮（改为绝对定位，不再堆叠） ==========
    // 以下按钮位置、大小可自行修改，互不重叠
    QPushButton *settingsBtn = new QPushButton("", this);
    settingsBtn->setGeometry(20, 20, 70, 70);
    settingsBtn->setStyleSheet(R"(
>>>>>>> d03f6bc (蛋)
        QPushButton {
            background-color: blue;
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

    // 按钮大小（你可以自行调整）
    int btnWidth = 200;
    int btnHeight = 110;

    // 定义按钮数据：文字、x坐标、y坐标、槽函数
    struct ButtonData {
        QString text;
        int x;
        int y;
        std::function<void()> onClick;
    };

    QList<ButtonData> buttons = {
        { "设置", 361, 498, [this]() { onSettings(); } },
        { "戳", 727, 336, [this]() { onPoke(); } },
        { "个人档案", 625, 498, [this]() { onProfile(); } },
        { "成就", 880, 498, [this]() { onAchievements(); } },
        { "返回主界面", 1025, 24, [this]() { onBackToMain(); } }
    };

    for (const auto &data : buttons) {
        QPushButton *btn = new QPushButton(data.text, this);
        btn->setFixedSize(btnWidth, btnHeight);
        btn->setStyleSheet(buttonStyle);
        btn->move(data.x, data.y);
        QObject::connect(btn, &QPushButton::clicked, data.onClick);
    }
}

SelectSongWindow::~SelectSongWindow() {}

void SelectSongWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // 加载背景图（请将 bg_select.png 放在工作目录，或修改路径）
    QPixmap bg("bg_select.png");
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), QColor(30, 30, 40));
        qDebug() << "背景图 bg_select.png 未找到，使用纯色背景";
    }
    QWidget::paintEvent(event);
}

<<<<<<< HEAD
=======
void SelectSongWindow::onSongCardClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int idx = m_cardButtons.indexOf(btn);
    if (idx < 0) return;

    QList<SongInfo> songs = GameConfig::instance()->getSongs();
    if (idx >= songs.size()) return;

    const SongInfo &song = songs[idx];
    qDebug() << "Song selected:" << song.name << "path:" << song.mapFolderPath;

    // 创建游戏场景并显示
    GameScene *game = new GameScene(song.mapFolderPath, this);
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->raise();
    game->show();
    this->hide();  // 隐藏选曲界面
}

// ---------- 以下为原有的功能按钮槽函数，保持不变 ----------
>>>>>>> d03f6bc (蛋)
void SelectSongWindow::onSettings()
{
    SettingsWindow *window = new SettingsWindow(this);
    window->show();
}

void SelectSongWindow::onPoke()
{
    PokeWindow *window = new PokeWindow(this);
    window->show();
}

void SelectSongWindow::onProfile()
{
    ProfileWindow *window = new ProfileWindow(this);
    window->show();
}

void SelectSongWindow::onAchievements()
{
    AchievementsWindow *window = new AchievementsWindow(this);
    window->show();
}

void SelectSongWindow::onBackToMain()
{
    // 关闭当前选曲窗口，并确保主窗口显示
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

#include "SelectSongWindow.h"
#include "SettingsWindow.h"
#include "PokeWindow.h"
#include "ProfileWindow.h"
#include "AchievementsWindow.h"
#include "MainWindow.h"
#include "gameconfig.h"
#include "gamescene.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QPainter>
#include <QDebug>

SelectSongWindow::SelectSongWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("选曲界面");
    setFixedSize(1200, 800);

    // ========== 左侧：歌曲网格区域（使用布局，不改为绝对定位） ==========
    QWidget *leftArea = new QWidget(this);
    leftArea->setGeometry(20, 100, 800, 600);  // 占据大部分区域，留出右侧和底部空间
    QVBoxLayout *leftLayout = new QVBoxLayout(leftArea);

    // 歌曲网格容器
    QWidget *gridContainer = new QWidget();
    //gridContainer->setStyleSheet("background: transparent;");
    QGridLayout *grid = new QGridLayout(gridContainer);
    grid->setSpacing(25);
    grid->setContentsMargins(10, 10, 10, 10);

    QList<SongInfo> songs = GameConfig::instance()->getSongs();
    if (songs.isEmpty()) {
        QLabel *noSongLabel = new QLabel("没有找到歌曲，请检查 maps 目录", gridContainer);
        noSongLabel->setStyleSheet("color: white; font-size: 20px;");
        grid->addWidget(noSongLabel, 0, 0);
    } else {
        const int cols = 4; // 每行4个卡片
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
                    border-radius: 12px;
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
                qDebug() << "封面缺失:" << song.coverPath;
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
    QPushButton *settingsBtn = new QPushButton("设置", this);
    settingsBtn->setGeometry(20, 20, 70, 70);
    settingsBtn->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(70,130,200,200);
            color: white;
            font-size: 16px;
            border-radius: 8px;
        }

    )");
    connect(settingsBtn, &QPushButton::clicked, this, &SelectSongWindow::onSettings);

    QPushButton *pokeBtn = new QPushButton("戳", this);
    pokeBtn->setGeometry(950, 190, 180, 60);
    pokeBtn->setStyleSheet(settingsBtn->styleSheet());
    connect(pokeBtn, &QPushButton::clicked, this, &SelectSongWindow::onPoke);

    QPushButton *profileBtn = new QPushButton("个人档案", this);
    profileBtn->setGeometry(950, 280, 180, 60);
    profileBtn->setStyleSheet(settingsBtn->styleSheet());
    connect(profileBtn, &QPushButton::clicked, this, &SelectSongWindow::onProfile);

    QPushButton *achieveBtn = new QPushButton("成就", this);
    achieveBtn->setGeometry(1110, 710, 70, 70);
    achieveBtn->setStyleSheet(settingsBtn->styleSheet());
    connect(achieveBtn, &QPushButton::clicked, this, &SelectSongWindow::onAchievements);

    QPushButton *backBtn = new QPushButton("返回主界面", this);
    backBtn->setGeometry(20, 710, 70, 70);
    backBtn->setStyleSheet(settingsBtn->styleSheet());
    connect(backBtn, &QPushButton::clicked, this, &SelectSongWindow::onBackToMain);

    // ========== 四张图片（绝对定位，位置大小可调，并且不拦截鼠标事件） ==========
    m_image1 = new QLabel(this);
    m_image1->setPixmap(QPixmap("s1.png"));
    m_image1->setScaledContents(true);
    m_image1->setGeometry(20, 20, 70, 70);
    m_image1->setAttribute(Qt::WA_TransparentForMouseEvents, true); // 鼠标穿透，不拦截点击
    m_image1->raise();

    m_image2 = new QLabel(this);
    m_image2->setPixmap(QPixmap("s2.png"));
    m_image2->setScaledContents(true);
    m_image2->setGeometry(20, 710, 70, 70);
    m_image2->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_image2->raise();

    m_image3 = new QLabel(this);
    m_image3->setPixmap(QPixmap("s3.png"));
    m_image3->setScaledContents(true);
    m_image3->setGeometry(800, 600, 150, 150);
    m_image3->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_image3->raise();

    m_image4 = new QLabel(this);
    m_image4->setPixmap(QPixmap("s4.png"));
    m_image4->setScaledContents(true);
    m_image4->setGeometry(1000, 50, 80, 80);
    m_image4->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_image4->raise();
}

SelectSongWindow::~SelectSongWindow() {}

void SelectSongWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg("bg_select.png");
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), QColor(30, 30, 40));
        qDebug() << "背景图 bg_select.png 未找到，使用纯色背景";
    }
    QWidget::paintEvent(event);
}

void SelectSongWindow::onSongCardClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int idx = m_cardButtons.indexOf(btn);
    if (idx < 0) return;

    QList<SongInfo> songs = GameConfig::instance()->getSongs();
    if (idx >= songs.size()) return;

    const SongInfo &song = songs[idx];
    qDebug() << "开始游戏:" << song.name << "谱面路径:" << song.mapFolderPath;

    // 创建游戏场景并显示
    GameScene *game = new GameScene(song.mapFolderPath, this);
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    this->hide();  // 隐藏选曲界面
}

// ---------- 以下为原有的功能按钮槽函数，保持不变 ----------
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
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

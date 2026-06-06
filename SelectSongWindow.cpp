#include "SelectSongWindow.h"
#include "SettingsWindow.h"
#include "PokeWindow.h"
#include "ProfileWindow.h"
#include "AchievementsWindow.h"
#include "MainWindow.h"
#include "gamescene.h"
#include "gameconfig.h"
#include "recordwindow.h"

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
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QVariant>

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

    // ==========================================
    // 【核心改造】：动态扫描本地 maps 文件夹 (跨平台绝对安全版)
    // ==========================================
    QString songsPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QDir macDir(songsPath);
    macDir.cdUp(); macDir.cdUp(); macDir.cdUp(); // 跳出 .app 黑盒
    songsPath = macDir.absolutePath();
#endif

    songsPath += "/maps"; // 拼接 maps 文件夹名

    // 【无敌调试法】：在终端里打印出大厅到底在扫哪个文件夹！
    qDebug() << "========================================";
    qDebug() << "【选曲大厅扫描路径】:" << songsPath;
    qDebug() << "========================================";

    QDir songsDir(songsPath);
    if (!songsDir.exists()) {
        songsDir.mkpath("."); // 如果连 maps 文件夹都没有，就建一个
    }

    // 获取目录下所有不含 . 和 .. 的子文件夹
    QStringList mapFolders = songsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    // ... 下面的 if (mapFolders.isEmpty()) 保持原样 ...
    if (mapFolders.isEmpty()) {
        QLabel *noSongLabel = new QLabel("NO TRACKS FOUND\n(请使用制谱工具生成，或放入 songs 文件夹)", gridContainer);
        noSongLabel->setStyleSheet("color: rgba(255,255,255,100); font-size: 24px; font-weight: bold;");
        grid->addWidget(noSongLabel, 0, 0);
    } else {
        int row = 0, col = 0;

        // 遍历找到的每一个谱面文件夹
        for (int i = 0; i < mapFolders.size(); ++i) {
            QString folderName = mapFolders[i];
            QString mapFolderPath = songsPath + "/" + folderName;

            // 默认属性
            QString songName = folderName; // 如果没读到名字，就用文件夹名字顶替
            QString coverPath = "";

            // 读取这个文件夹里的 info.txt
            QFile infoFile(mapFolderPath + "/info.txt");
            if (infoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&infoFile);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    // 加入 Qt::CaseInsensitive，大小写 SongName 都能认！
                    if (line.startsWith("SongName:", Qt::CaseInsensitive)) {
                        songName = line.mid(9).trimmed();
                    } else if (line.startsWith("CoverFile:", Qt::CaseInsensitive)) {
                        QString cName = line.mid(10).trimmed();
                        // 【修复】：删掉了那个奇葩的 != "cover.jpg" 判断
                        if (!cName.isEmpty()) {
                            coverPath = mapFolderPath + "/" + cName;
                        }
                    }
                }
                infoFile.close();
            }

            // 【新增黑科技】：智能保底机制。
            // 如果 info.txt 里没写曲绘，或者写错了，我们自动去文件夹里找！
            if (coverPath.isEmpty() || !QFile::exists(coverPath)) {
                if (QFile::exists(mapFolderPath + "/cover.jpg")) {
                    coverPath = mapFolderPath + "/cover.jpg";
                } else if (QFile::exists(mapFolderPath + "/cover.png")) {
                    coverPath = mapFolderPath + "/cover.png";
                }
            }

            // --- 开始生成 UI 卡片 ---
            QPushButton *card = new QPushButton();
            card->setFixedSize(250, 120);

            // 【黑科技】：直接把这个谱面的真实路径绑在按钮身上！
            card->setProperty("mapFolderPath", mapFolderPath);

            // 封面处理
            QString bgStyle;
            if (!coverPath.isEmpty() && QFile::exists(coverPath)) {
                bgStyle = QString("background-image: url(%1); background-position: center;").arg(coverPath);
            } else {
                bgStyle = "background-color: rgba(30, 40, 55, 200);"; // 纯色垫底
            }

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
                    background-color: rgba(60, 80, 100, 250);
                }
            )").arg(bgStyle));

            // ==========================================
            // 【UI 升级】：智能文字截断与悬浮提示
            // ==========================================
            // 1. 设置悬浮提示（鼠标放上去会弹出一个小黑框显示全名）
            card->setToolTip(songName);

            // 2. 使用文字测量仪，模拟 18px 粗体的大小
            QFont tempFont("Arial", 14, QFont::Bold);
            QFontMetrics fm(tempFont);

            // 3. 如果名字宽度超过 190 像素（卡片宽 250），自动切断并加上 "..."
            QString elidedName = fm.elidedText(songName, Qt::ElideRight, 190);

            card->setText(elidedName); // 显示处理过的歌名
            // ==========================================
            connect(card, &QPushButton::clicked, this, &SelectSongWindow::onSongCardClicked);

            // ... 上面是你的生成卡片代码 ...
            grid->addWidget(card, row, col);
            col++;
            if (col >= 2) { col = 0; row++; }
        }
    } // <--- 这是 else 的结束括号

    // ==========================================
    // 【漏掉的核心修复】：把装满歌曲的容器放到左侧屏幕上！
    // ==========================================
    QScrollArea *scrollArea = new QScrollArea(leftArea);
    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    // 把滚动区域正式加入左侧大布局！
    leftLayout->addWidget(scrollArea);

    // ==========================================
    // 3. 右侧：纯文字排版 + 极简按钮区
    // ==========================================
    QWidget *rightArea = new QWidget(this);
    // ... 后面的代码保持不变 ...

    QVBoxLayout *rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(40, 0, 0, 0);
    rightLayout->setAlignment(Qt::AlignTop | Qt::AlignRight);

    // 返回主菜单按钮 (纯文字)
    QPushButton *backBtn = new QPushButton("◀ BACK", rightArea);
    backBtn->setFixedSize(120, 40);
    backBtn->setStyleSheet("QPushButton { background: transparent; color: rgba(255,255,255,150); font-size: 18px; font-weight: bold; border: none; text-align: right; } QPushButton:hover { color: white; }");
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        // 1. 召唤一个全新的初始主界面
        MainWindow* startMenu = new MainWindow();
        startMenu->setAttribute(Qt::WA_DeleteOnClose);
        startMenu->show(); // 或者是 startMenu->showFullScreen();

        // 2. 把当前的选曲大厅彻底销毁
        this->close();
    });
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

    // ==========================================
    // 【新增】：制谱器入口按钮
    // ==========================================
    QPushButton *recordBtn = new QPushButton("EDITOR / 制谱", rightArea);
    recordBtn->setFixedSize(250, 50);
    // 给制谱按钮换个稍微亮一点的边框颜色，彰显它的特殊地位！
    QString editorStyle = menuBtnStyle;
    editorStyle.replace("rgba(255,255,255,50)", "#32CD32");
    recordBtn->setStyleSheet(editorStyle);
    connect(recordBtn, &QPushButton::clicked, this, &SelectSongWindow::onRecord);
    rightLayout->addWidget(recordBtn, 0, Qt::AlignRight);
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

    // 直接从被点击的按钮身上，把路径“撕”下来
    QString mapPath = btn->property("mapFolderPath").toString();
    if (mapPath.isEmpty()) return;

    // 召唤游戏场景
    GameScene *game = new GameScene(mapPath);
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->showFullScreen(); // 【架构同步】：统一使用全屏

    this->hide();
    this->deleteLater(); // 【架构同步】：安全销毁当前选曲窗口
}

void SelectSongWindow::onSettings() { SettingsWindow *window = new SettingsWindow(this); window->show(); }
void SelectSongWindow::onPoke() { PokeWindow *window = new PokeWindow(this); window->show(); }
void SelectSongWindow::onProfile() { ProfileWindow *window = new ProfileWindow(this); window->show(); }
void SelectSongWindow::onAchievements() { AchievementsWindow *window = new AchievementsWindow(this); window->show(); }
void SelectSongWindow::onBackToMain() { this->close(); if (parentWidget()) parentWidget()->show(); }


// 【新增】：跳转到制谱仪的接力连招
void SelectSongWindow::onRecord()
{
    RecordWindow *recordWin = new RecordWindow();
    recordWin->setAttribute(Qt::WA_DeleteOnClose);
    recordWin->showFullScreen();

    this->hide();
    this->deleteLater(); // 安全销毁大厅
}

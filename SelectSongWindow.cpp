#include "SelectSongWindow.h"
#include "SettingsWindow.h"
#include "PokeWindow.h"
#include "ProfileWindow.h"
#include "AchievementsWindow.h"
#include "MainWindow.h"
#include "gamescene.h"
#include "gameconfig.h"
#include "recordwindow.h"
#include "scenemanager.h"

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
#include <QScrollBar> //

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
    QLabel *listTitle = new QLabel("SONGS", leftArea);
    listTitle->setStyleSheet("color: #00BFFF; font-size: 32px; font-weight: 900; letter-spacing: 5px;");
    leftLayout->addWidget(listTitle);

    QFrame *line = new QFrame(leftArea);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: rgba(0, 191, 255, 100);");
    leftLayout->addWidget(line);

    // ==========================================
    // 歌曲列表容器 (垂直单列排版)
    // ==========================================
    QWidget *listContainer = new QWidget();
    listContainer->setStyleSheet("background: transparent;");
    QVBoxLayout *vbox = new QVBoxLayout(listContainer);
    vbox->setSpacing(15);
    vbox->setContentsMargins(0, 10, 10, 10);
    vbox->setAlignment(Qt::AlignTop); // 保证列表里的歌从上往下整齐堆叠

    // ==========================================
    // 动态扫描本地 maps 文件夹
    // ==========================================
    QString songsPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QDir macDir(songsPath);
    macDir.cdUp(); macDir.cdUp(); macDir.cdUp(); // 跳出 .app 黑盒
    songsPath = macDir.absolutePath();
#endif

    songsPath += "/maps"; // 拼接 maps 文件夹名

    // // 在终端里打印出大厅到底在扫哪个文件夹！
    // qDebug() << "========================================";
    // qDebug() << "【选曲大厅扫描路径】:" << songsPath;
    // qDebug() << "========================================";

    QDir songsDir(songsPath);
    if (!songsDir.exists()) {
        songsDir.mkpath("."); // 如果连 maps 文件夹都没有，就建一个
    }

    // 获取目录下所有不含 . 和 .. 的子文件夹
    QStringList mapFolders = songsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (mapFolders.isEmpty()) {
        QLabel *noSongLabel = new QLabel("NO TRACKS FOUND\n(请使用制谱工具生成，或放入 maps 文件夹)", listContainer);
        noSongLabel->setStyleSheet("color: rgba(255,255,255,100); font-size: 24px; font-weight: bold;");
        vbox->addWidget(noSongLabel);
    } else {
        // 遍历找到的每一个谱面文件夹
        for (int i = 0; i < mapFolders.size(); ++i) {
            QString folderName = mapFolders[i];
            QString mapFolderPath = songsPath + "/" + folderName;

            QString songName = folderName;
            QString coverPath = "";

            // ==========================================
            // 安全读取 info.txt
            // ==========================================
            QFile infoFile(mapFolderPath + "/info.txt");
            if (infoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&infoFile);
                int lineCount = 0;
                // 限制：最多只读 50 行，防止读取到几百 MB 的假文本/乱码文件导致死机
                while (!in.atEnd() && lineCount < 50) {
                    QString line = in.readLine(1024); // 每行最多读 1024 个字符
                    if (line.startsWith("SongName:", Qt::CaseInsensitive)) {
                        songName = line.mid(9).trimmed();
                    } else if (line.startsWith("CoverFile:", Qt::CaseInsensitive)) {
                        QString cName = line.mid(10).trimmed();
                        if (!cName.isEmpty()) coverPath = mapFolderPath + "/" + cName;
                    }
                    lineCount++;
                }
                infoFile.close();
            }

            // 保底图片
            if (coverPath.isEmpty() || !QFile::exists(coverPath)) {
                if (QFile::exists(mapFolderPath + "/cover.jpg")) coverPath = mapFolderPath + "/cover.jpg";
                else if (QFile::exists(mapFolderPath + "/cover.png")) coverPath = mapFolderPath + "/cover.png";
            }

            // UI 框架
            QWidget *rowWidget = new QWidget();
            rowWidget->setFixedSize(520, 100);

            QPushButton *card = new QPushButton(rowWidget);
            card->setFixedSize(520, 100);
            card->setProperty("mapFolderPath", mapFolderPath);

            card->setStyleSheet(R"(
                QPushButton {
                    background-color: rgba(20, 25, 35, 180);
                    border-left: 5px solid #4682B4;
                    border-top: 1px solid rgba(255,255,255,20);
                    border-right: 1px solid rgba(255,255,255,20);
                    border-bottom: 1px solid rgba(255,255,255,20);
                    border-radius: 12px;
                    color: white;
                    font-size: 24px;
                    font-weight: bold;
                    text-align: left;
                    padding-left: 110px;
                }
                QPushButton:hover {
                    border-left: 5px solid #00BFFF;
                    background-color: rgba(50, 70, 90, 200);
                }
            )");

            card->setToolTip(songName);
            QFont tempFont("Arial", 24, QFont::Bold);
            QFontMetrics fm(tempFont);
            QString elidedName = fm.elidedText(songName, Qt::ElideRight, 360);
            card->setText(elidedName);

            connect(card, &QPushButton::clicked, this, &SelectSongWindow::onSongCardClicked);

            // ==========================================
            // 曲绘渲染
            // ==========================================
            QLabel *coverLabel = new QLabel(rowWidget);
            coverLabel->setGeometry(10, 10, 80, 80);
            coverLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

            bool imageLoaded = false;
            if (!coverPath.isEmpty() && QFile::exists(coverPath)) {
                QPixmap src;
                // 使用 load() 替代直接构造，能安全捕获损坏的图片格式
                if (src.load(coverPath)) {
                    // 确保图片有实体，且宽高不是 0
                    if (!src.isNull() && src.width() > 0 && src.height() > 0) {

                        // 如果原图太大（比如4K图），在裁剪前先粗略压扁它，防止内存溢出
                        if (src.width() > 2000 || src.height() > 2000) {
                            src = src.scaled(1000, 1000, Qt::KeepAspectRatio, Qt::FastTransformation);
                        }

                        int renderSize = 160;
                        QPixmap scaled = src.scaled(renderSize, renderSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

                        // 确保缩放成功，并且尺寸达标，防止 copy() 时发生越界崩溃
                        if (!scaled.isNull() && scaled.width() >= renderSize && scaled.height() >= renderSize) {
                            int x = (scaled.width() - renderSize) / 2;
                            int y = (scaled.height() - renderSize) / 2;
                            scaled = scaled.copy(x, y, renderSize, renderSize);

                            QPixmap rounded(renderSize, renderSize);
                            rounded.fill(Qt::transparent);
                            QPainter p(&rounded);
                            p.setRenderHint(QPainter::Antialiasing);
                            p.setRenderHint(QPainter::SmoothPixmapTransform);

                            QPainterPath clipPath;
                            clipPath.addRoundedRect(0, 0, renderSize, renderSize, 16, 16);
                            p.setClipPath(clipPath);
                            p.drawPixmap(0, 0, scaled);
                            p.end();

                            coverLabel->setPixmap(rounded);
                            coverLabel->setScaledContents(true);
                            imageLoaded = true; // 标记加载成功
                        }
                    }
                }
            }

            // 使用默认纯色兜底
            if (!imageLoaded) {
                coverLabel->setStyleSheet("background-color: rgba(255, 255, 255, 15); border-radius: 8px;");
            }

            vbox->addWidget(rowWidget);

        }
    } // <--- 这是 else 的结束括号

    // ==========================================
    // 放入滚动区域
    // ==========================================
    QScrollArea *scrollArea = new QScrollArea(leftArea);
    scrollArea->setWidget(listContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    // 让鼠标滚轮每次滚动的距离更长
    scrollArea->verticalScrollBar()->setSingleStep(30);


    // 把滚动区域加入左侧大布局
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
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        // 交给舞台去切换
        MainWindow* startMenu = new MainWindow();
        SceneManager::switchScene(startMenu);
    });
    rightLayout->addWidget(backBtn, 0, Qt::AlignRight);

    rightLayout->addSpacing(80); // 留白

    // 超大背景水印排版
    QLabel *hugeText = new QLabel("MEOWTHM\nSYSTEM", rightArea);
    hugeText->setAlignment(Qt::AlignRight);
    hugeText->setStyleSheet("color: rgba(255, 255, 255, 50); font-size: 80px; font-weight: 900; line-height: 80px;");
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
    // 制谱器入口按钮
    // ==========================================
    QPushButton *recordBtn = new QPushButton("EDITOR / 制谱", rightArea);
    recordBtn->setFixedSize(250, 50);
    // 给制谱按钮换个稍微亮一点的边框颜色
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
// 终极赛博朋克背景：高清底图 + HUD 几何光效
// ==========================================
void SelectSongWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // ==========================================
    // 1. 绘制底层：素材图
    // ==========================================

    QPixmap bg(":/image/images/bg_select.jpg");
    if (!bg.isNull()) {
        painter.drawPixmap(this->rect(), bg); // 拉伸铺满全屏
    } else {
        // 如果图没加载出来，保底一个深色
        painter.fillRect(rect(), QColor(10, 15, 25));
    }

    // ==========================================
    // 2. 环境压暗层
    // ==========================================
    // 给底图盖上一层 60% 不透明度的深渊黑，把前方 UI 凸显出来
    painter.fillRect(rect(), QColor(10, 15, 25, 150));

    // ==========================================
    // 3. 绘制左侧歌单区的“专属舞台光”
    // ==========================================
    QLinearGradient leftGlow(0, 0, 600, 0);
    leftGlow.setColorAt(0.0, QColor(0, 191, 255, 40)); // 左边缘泛出科技蓝光
    leftGlow.setColorAt(1.0, QColor(0, 191, 255, 0));  // 向右侧渐隐消失
    painter.fillRect(0, 0, 600, height(), leftGlow);

    // ==========================================
    // 4. 绘制右侧巨型多边形切割光带
    // ==========================================
    QPainterPath path;
    path.moveTo(width() * 0.65, 0);
    path.lineTo(width() * 0.90, 0);
    path.lineTo(width(), height() * 0.4);
    path.lineTo(width(), height());
    path.lineTo(width() * 0.45, height());
    path.closeSubpath();

    QLinearGradient shapeGradient(width() * 0.5, 0, width(), height());
    shapeGradient.setColorAt(0.0, QColor(0, 191, 255, 25));   // 霓虹蓝
    shapeGradient.setColorAt(0.5, QColor(138, 43, 226, 20));  // 赛博紫
    shapeGradient.setColorAt(1.0, QColor(255, 255, 255, 5));
    painter.fillPath(path, shapeGradient);

    // 沿切割边缘画一条锐利的激光线
    painter.setPen(QPen(QColor(0, 191, 255, 120), 2));
    painter.drawLine(width() * 0.65, 0, width() * 0.45, height());

    // ==========================================
    // 5. 机甲 HUD 取景器边框
    // ==========================================
    painter.setPen(QPen(QColor(255, 255, 255, 80), 3));
    int margin = 20;
    int lineLen = 40;

    // 左上角
    painter.drawLine(margin, margin, margin + lineLen, margin);
    painter.drawLine(margin, margin, margin, margin + lineLen);
    // 右下角
    painter.drawLine(width() - margin, height() - margin, width() - margin - lineLen, height() - margin);
    painter.drawLine(width() - margin, height() - margin, width() - margin, height() - margin - lineLen);

    QWidget::paintEvent(event);
}

void SelectSongWindow::onSongCardClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString mapPath = btn->property("mapFolderPath").toString();
    if (mapPath.isEmpty()) return;

    // 全交给舞台
    GameScene *game = new GameScene(mapPath);
    SceneManager::switchScene(game);
}

void SelectSongWindow::onSettings() { SettingsWindow *window = new SettingsWindow(this); window->show(); }
void SelectSongWindow::onPoke() { PokeWindow *window = new PokeWindow(this); window->show(); }
void SelectSongWindow::onProfile() { ProfileWindow *window = new ProfileWindow(this); window->show(); }
void SelectSongWindow::onAchievements() { AchievementsWindow *window = new AchievementsWindow(this); window->show(); }
void SelectSongWindow::onBackToMain()
{
    MainWindow* startMenu = new MainWindow();
    SceneManager::switchScene(startMenu);
}


void SelectSongWindow::onRecord()
{
    RecordWindow *recordWin = new RecordWindow();
    SceneManager::switchScene(recordWin);
}

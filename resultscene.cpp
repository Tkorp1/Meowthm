#include "resultscene.h"
#include <QFontDatabase>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include "mainwindow.h"
#include "AnalysisWindow.h"

ResultScene::ResultScene(const GameState& _state, QWidget *parent)
    : QWidget(parent), state(_state)
{
    // =========================
    // 1. 字体加载
    // =========================
    QString fontFamily = "Arial"; // 默认保底字体
    int fontId = QFontDatabase::addApplicationFont(":/static/Orbitron-Bold.ttf");
    if (fontId != -1) {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    }

    // =========================
    // 2. 动态获取曲绘与谱面路径
    // =========================
    QString coverPath;
    QString mapPath;
    for (const SongInfo& s : GameConfig::instance()->getSongs()) {
        if (s.name == state.getCurrentSong()) {
            coverPath = s.coverPath;
            mapPath = s.mapFolderPath;
            break;
        }
    }

    // 绘制曲绘面板 (x=700, y=70)
    QLabel* coverLabel = new QLabel(this);
    coverLabel->setGeometry(700, 70, 200, 200);
    QPixmap cover(coverPath);
    if (!cover.isNull()) {
        // 使用 border-image 让图片完美贴合并支持圆角裁切
        coverLabel->setStyleSheet(QString("border-image: url(%1); border-radius: 15px; border: 3px solid rgba(218, 112, 214, 150);").arg(coverPath));
    } else {
        // 如果没有曲绘，显示一个高级的文字占位符
        coverLabel->setText("NO COVER");
        coverLabel->setAlignment(Qt::AlignCenter);
        coverLabel->setStyleSheet("color: rgba(255,255,255,100); font-weight: bold; font-size: 24px; background-color: rgba(0,0,0,100); border: 3px solid rgba(218, 112, 214, 150); border-radius: 15px;");
    }

    // =========================
    // 3. 标题 (Song Name) -> 向右推移到曲绘旁边 (x=930)
    // =========================
    songNameLabel = new QLabel(this);
    songNameLabel->setText(QString(state.getCurrentSong()));
    songNameLabel->setGeometry(930, 70, 600, 60);
    songNameLabel->setStyleSheet("color: white; font-size: 52px; font-weight: 900; letter-spacing: 3px; background: transparent;");

    // =========================
    // 4. 最终得分 (Total Score) -> 向右推移
    // =========================
    QLabel* scoreTitle = new QLabel("TOTAL SCORE", this);
    scoreTitle->setGeometry(930, 150, 300, 30);
    scoreTitle->setStyleSheet("color: #DDA0DD; font-size: 22px; font-weight: bold; letter-spacing: 5px; background: transparent;");

    scoreLabel = new QLabel(this);
    scoreLabel->setText(QString("%1").arg(state.getCurrentScore(), 7, 10, QChar('0')));
    scoreLabel->setGeometry(930, 170, 600, 100);
    scoreLabel->setFont(QFont(fontFamily, 90, QFont::Bold));
    scoreLabel->setStyleSheet("color: #FFFFFF; background: transparent;");

    // =========================
    // 5. 连击 (Max Combo) 与 准确率 (Acc) -> 保持在原本的左侧基准线 (x=700)
    // =========================
    comboLabel = new QLabel(this);
    comboLabel->setText(QString("<div style='color:#A9A9A9; font-size:20px; font-weight:bold;'>MAX COMBO</div>"
                                "<div style='color:white; font-size:45px; font-family:\"%1\";'>%2</div>")
                            .arg(fontFamily).arg(state.getMaxCombo()));
    comboLabel->setGeometry(700, 310, 300, 90);
    comboLabel->setStyleSheet("background: transparent;");

    accLabel = new QLabel(this);
    accLabel->setText(QString("<div style='color:#A9A9A9; font-size:20px; font-weight:bold;'>ACCURACY</div>"
                              "<div style='color:white; font-size:45px; font-family:\"%1\";'>%2%</div>")
                          .arg(fontFamily).arg(state.getCurrentAcc() * 100, 0, 'f', 2));
    accLabel->setGeometry(1050, 310, 300, 90);
    accLabel->setStyleSheet("background: transparent;");

    // =========================
    // 6. 详细判定 (Perfect, Good, Miss)
    // =========================
    QString statHtml = "<table width='600'><tr>"
                       "<td align='left' style='color:%1; font-size:28px; font-weight:900; letter-spacing: 2px;'>%2</td>"
                       "<td align='right' style='color:white; font-size:40px; font-family:\"%3\";'>%4</td>"
                       "</tr></table>";

    perfectLabel = new QLabel(this);
    perfectLabel->setGeometry(700, 440, 600, 50);
    perfectLabel->setText(statHtml.arg("#00FFFF", "PERFECT", fontFamily, QString::number(state.getCurrentPerfect())));
    perfectLabel->setStyleSheet("background: transparent;");

    goodLabel = new QLabel(this);
    goodLabel->setGeometry(700, 510, 600, 50);
    goodLabel->setText(statHtml.arg("#32CD32", "GOOD", fontFamily, QString::number(state.getCurrentGood())));
    goodLabel->setStyleSheet("background: transparent;");

    missLabel = new QLabel(this);
    missLabel->setGeometry(700, 580, 600, 50);
    missLabel->setText(statHtml.arg("#FF0055", "MISS", fontFamily, QString::number(state.getCurrentMiss())));
    missLabel->setStyleSheet("background: transparent;");

    // =========================
    // 7. 底部双按钮 (重试 / 结束)
    // =========================
    // [新增] 神经链接分析报告按钮：极具科幻感
    QPushButton* reportButton = new QPushButton("GENERATE YOUR MEOWTHM REPORT / 生成你的分析报告", this);
    reportButton->setGeometry(700, 780, 600, 50);
    reportButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(0, 255, 255, 20);
            border: 1px solid #00FFFF;
            color: #00FFFF;
            font-size: 18px;
            font-weight: bold;
            letter-spacing: 3px;
        }
        QPushButton:hover { background-color: #00FFFF; color: black; border: 2px solid white; }
    )");

    connect(reportButton, &QPushButton::clicked, this, [this]() {
        // 【核心修改】：在 new 的时候传入 `this`，让它成为当前界面的子图层！
        AnalysisWindow* aw = new AnalysisWindow(state, this);
        aw->setGeometry(0, 0, this->width(), this->height()); // 绝对覆盖全屏
        aw->raise(); // 强制提升到最顶层，盖住所有按钮和文字
        aw->show();
    });

    // [新增] 重试按钮：科技蓝色调
    QPushButton* restartButton = new QPushButton("RETRY / 重试", this);
    restartButton->setGeometry(700, 660, 260, 60);
    restartButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid #00BFFF;
            border-radius: 30px;
            color: #00BFFF;
            font-size: 22px;
            font-weight: bold;
            letter-spacing: 2px;
        }
        QPushButton:hover { background-color: #00BFFF; color: white; border: 2px solid white; }
    )");

    // 【核心魔法】：使用 Lambda 匿名函数，无需修改 .h 头文件即可实现重新开局！
    connect(restartButton, &QPushButton::clicked, this, [this, mapPath]() {
        if (!mapPath.isEmpty()) {
            GameScene* game = new GameScene(mapPath);
            game->setAttribute(Qt::WA_DeleteOnClose);
            game->show();
            this->close(); // 关闭当前结算界面
        }
    });

    // 结束按钮：霓虹紫色调
    returnButton = new QPushButton("FINISH / 结束", this);
    returnButton->setGeometry(1040, 660, 260, 60); // 放在重试按钮右侧
    returnButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid #DDA0DD;
            border-radius: 30px;
            color: #DDA0DD;
            font-size: 22px;
            font-weight: bold;
            letter-spacing: 2px;
        }
        QPushButton:hover { background-color: #DDA0DD; color: black; border: 2px solid white; }
    )");
    connect(returnButton, &QPushButton::clicked, this, &ResultScene::onReturnMainMenu);

    // =========================
    // 8. 组装完毕，统一全屏展示
    // =========================
    resize(1600, 900);
    showFullScreen();
}

ResultScene::~ResultScene()
{
}

void ResultScene::onReturnMainMenu()
{
    MainWindow* menu = new MainWindow();
    menu->show();
    close();
}

// =========================
// 极具科幻感的背景与面板绘制
// =========================
void ResultScene::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. 绘制背景原图
    QPixmap bg(":/image/images/resultScene.jpg");
    painter.drawPixmap(this->rect(), bg);

    // 2. 绘制右侧倾斜的磨砂暗色面板 (留出左侧光环)
    QPainterPath panelPath;
    panelPath.moveTo(600, 0);       // 顶部起点偏右
    panelPath.lineTo(1600, 0);      // 顶部拉满
    panelPath.lineTo(1600, 900);    // 底部拉满
    panelPath.lineTo(400, 900);     // 底部起点更偏左，形成锐利的斜角
    panelPath.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(15, 10, 25, 210)); // 深紫色半透明，极其契合背景
    painter.drawPath(panelPath);

    // 3. 画龙点睛：在倾斜面板的边缘，画一条发光的霓虹紫粉色线
    QPen neonPen(QColor(218, 112, 214, 200)); // 霓虹紫粉色
    neonPen.setWidth(4);
    painter.setPen(neonPen);
    painter.drawLine(600, 0, 400, 900);
}

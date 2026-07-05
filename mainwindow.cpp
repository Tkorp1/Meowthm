#include "MainWindow.h"
#include "SelectSongWindow.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPainterPath>
#include <QFont>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)
    , titleLabel(nullptr)
    , currentFrame(0)
    , totalFrames(32)
    , columns(8)
    , frameWidth(0)
    , frameHeight(0)
{
    this->setFixedSize(1200, 800);


    spriteSheet.load(":/image/big_image.png");

    frameWidth = spriteSheet.width() / columns;
    frameHeight = spriteSheet.height() / (totalFrames / columns);


    // 2. 初始化动画定时器
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, [this]() {
        currentFrame = (currentFrame + 1) % totalFrames;
        this->update();
    });
    animTimer->start(33);

    // 3. 创建子控件并配置样式
    titleLabel = new QLabel(this);
    titleLabel->setText("Me   wthm");
    titleLabel->setStyleSheet("background: transparent; color: rgba(200, 180, 255, 220);");
    QFont font("Arial Black", 180, QFont::Bold);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 5);
    titleLabel->setFont(font);
    titleLabel->adjustSize();

    setFocus();
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (titleLabel) {
        int x = (this->width() - titleLabel->width()) / 2;
        int y = (this->height() - titleLabel->height()) / 2;
        titleLabel->move(x, y);
        titleLabel->raise();
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // ========== 1. 极暗紫黑基底 ==========
    painter.fillRect(rect(), QColor(8, 6, 12));

    // ========== 2. 主渐变 ==========
    QLinearGradient bgGrad(0, 0, width(), height());
    bgGrad.setColorAt(0.0, QColor(15, 5, 25));
    bgGrad.setColorAt(0.4, QColor(40, 15, 50));
    bgGrad.setColorAt(0.7, QColor(25, 10, 40));
    bgGrad.setColorAt(1.0, QColor(8, 3, 18));
    painter.fillRect(rect(), bgGrad);

    // ========== 3. 霓虹光晕 ==========
    QRadialGradient glow1(QPointF(120, 100), 350);
    glow1.setColorAt(0.0, QColor(150, 50, 180, 90));
    glow1.setColorAt(0.5, QColor(100, 30, 130, 40));
    glow1.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(rect(), glow1);

    QRadialGradient glow2(QPointF(width()-100, height()-80), 400);
    glow2.setColorAt(0.0, QColor(200, 70, 150, 70));
    glow2.setColorAt(0.6, QColor(120, 40, 100, 25));
    glow2.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(rect(), glow2);

    // 压暗角落
    QRadialGradient darkCorner1(QPointF(0, 0), 400);
    darkCorner1.setColorAt(0.0, QColor(0, 0, 0, 180));
    darkCorner1.setColorAt(0.6, QColor(0, 0, 0, 80));
    darkCorner1.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(rect(), darkCorner1);

    QRadialGradient darkCorner2(QPointF(width(), height()), 450);
    darkCorner2.setColorAt(0.0, QColor(0, 0, 0, 200));
    darkCorner2.setColorAt(0.5, QColor(0, 0, 0, 60));
    darkCorner2.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(rect(), darkCorner2);

    QRadialGradient darkMidBottom(QPointF(width()/2, height()), 300);
    darkMidBottom.setColorAt(0.0, QColor(0, 0, 0, 100));
    darkMidBottom.setColorAt(0.7, QColor(0, 0, 0, 30));
    darkMidBottom.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(rect(), darkMidBottom);

    // ========== 精灵图切片动画绘制 ==========
    if (!spriteSheet.isNull()) {
        int col = currentFrame % columns;
        int row = currentFrame / columns;

        QRect sourceRect(col * frameWidth, row * frameHeight, frameWidth, frameHeight);

        int targetW = 220;
        int targetH = 220;
        int targetX = (width() - targetW) / 2;
        int targetY = (height() - targetH) / 2;
        QRect targetRect(targetX-135, targetY+10, targetW, targetH);

        painter.drawPixmap(targetRect, spriteSheet, sourceRect);
    } else {
        // 如果图层在绘制时发现图是空的，打印静态警告
        static bool warned = false;
        if (!warned) {
            qDebug() << "[运行时警告] paintEvent 检测到 spriteSheet 为空，跳过动画绘制。";
            warned = true;
        }
    }

    // ========== 4. 霓虹线条 ==========
    painter.setPen(QPen(QColor(170, 70, 220, 100), 1.5));
    painter.drawLine(0, height()*0.2, width()*0.3, 0);
    painter.drawLine(width()*0.1, height(), width(), height()*0.3);
    painter.drawLine(width()*0.6, height()*0.1, width()*0.85, height()*0.1);
    painter.drawLine(width()*0.2, height()*0.85, width()*0.5, height()*0.85);
    painter.drawLine(width()*0.85, height()*0.4, width()*0.85, height()*0.65);
    painter.drawLine(width()*0.1, height()*0.2, width()*0.1, height()*0.45);

    // 光带
    QPainterPath strip;
    strip.moveTo(0, height()*0.5);
    strip.lineTo(width()*0.25, height()*0.45);
    strip.lineTo(width()*0.28, height()*0.55);
    strip.lineTo(0, height()*0.6);
    strip.closeSubpath();
    painter.fillPath(strip, QColor(170, 70, 220, 30));

    QPainterPath strip2;
    strip2.moveTo(width()*0.7, height());
    strip2.lineTo(width()*0.85, height()*0.7);
    strip2.lineTo(width()*0.95, height()*0.72);
    strip2.lineTo(width(), height());
    strip2.closeSubpath();
    painter.fillPath(strip2, QColor(200, 80, 150, 25));

    // 光点
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(160, 70, 200, 100));
    painter.drawEllipse(60, 60, 5, 5);
    painter.drawEllipse(width()-80, height()-70, 7, 7);
    painter.setBrush(QColor(200, 90, 160, 80));
    painter.drawEllipse(280, height()-140, 4, 4);
    painter.drawEllipse(width()/2, 50, 4, 4);
}

void MainWindow::onSelectSong()
{
    // 召唤选曲大厅
    SelectSongWindow *window = new SelectSongWindow();

    // 交给舞台去切换
    SceneManager::switchScene(window);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        this->close();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    onSelectSong();
}

#include "AnalysisWindow.h"
#include <QPainter>
#include <QPainterPath>
#include <QLabel>
#include <qmath.h>
#include <QDebug>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

AnalysisWindow::AnalysisWindow(const GameState& state, QWidget *parent)
    : QWidget(parent), m_state(state)
{
    // 强制窗口全屏且置顶，并有半透明背景
    if (parent) {
        resize(parent->width(), parent->height());
    } else {
        resize(1600, 900); // 保底大小
    }
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(opacityEffect);
    QPropertyAnimation *anim = new QPropertyAnimation(opacityEffect, "opacity");
    anim->setDuration(100); // 动画时长
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    calculateMetrics();

    int dx = (width() - 1600) / 2;
    int dy = (height() - 900) / 2;

    // 关闭按钮
    m_closeBtn = new QPushButton("× CLOSE REPORT", this);
    m_closeBtn->setGeometry(650 + dx, 800 + dy, 300, 50);
    m_closeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid #00FFFF;
            color: #00FFFF; font-size: 20px; font-weight: bold; border-radius: 25px;
        }
        QPushButton:hover { background-color: #00FFFF; color: black; }
    )");
    connect(m_closeBtn, &QPushButton::clicked, this, &AnalysisWindow::close);

    // 添加一些文本标签展示结果
    QString infoHtml = QString(
                           "<div style='color:#00BFFF; font-size:24px; font-weight:bold;'>"
                           "REACTION RATE (反应速率): <span style='color:white;'>%1 ms %2</span><br><br>"
                           "STABILITY (稳定指数): <span style='color:white;'>±%3 ms</span><br><br>"
                           "PEAK KPS (极限爆发力): <span style='color:white;'>%4 Keys/Sec</span><br><br>"
                           "HEMISPHERE LOAD (左右协调率): <span style='color:white;'>L %5% / R %6%</span>"
                           "</div>"
                           ).arg(m_avgOffset, 0, 'f', 1)
                           .arg(m_avgOffset < 0 ? "(EARLY)" : "(LATE)")
                           .arg(m_stdDev, 0, 'f', 1)
                           .arg(m_peakKPS)
                           .arg(m_leftRatio * 100, 0, 'f', 1)
                           .arg(m_rightRatio * 100, 0, 'f', 1);

    QLabel* infoLabel = new QLabel(this);
    infoLabel->setGeometry(150 + dx, 550 + dy, 800, 250);
    infoLabel->setText(infoHtml);
    infoLabel->setStyleSheet("background: transparent;");
}

AnalysisWindow::~AnalysisWindow() {}

// ==========================================
// 核心数学引擎：计算高阶数据
// ==========================================
void AnalysisWindow::calculateMetrics()
{
    QList<qint64> offsets = m_state.getHitOffsets();
    QList<qint64> times = m_state.getHitTimestamps();
    int count = offsets.size();

    // 1. 容错：如果没打中任何音符
    if (count == 0) {
        m_avgOffset = 0; m_stdDev = 0; m_peakKPS = 0;
        m_leftRatio = 0.5; m_rightRatio = 0.5;
        return;
    }

    // 2. 计算平均偏差
    double sum = 0;
    for (qint64 o : offsets) sum += o;
    m_avgOffset = sum / count;

    // 3. 计算标准差 (方差的平方根，反映离散程度)
    double varianceSum = 0;
    for (qint64 o : offsets) varianceSum += (o - m_avgOffset) * (o - m_avgOffset);
    m_stdDev = std::sqrt(varianceSum / count);

    // 4. 计算峰值 KPS (1秒滑动窗口)
    m_peakKPS = 0;
    for (int i = 0; i < times.size(); ++i) {
        int currentKPS = 0;
        for (int j = i; j < times.size(); ++j) {
            if (times[j] - times[i] <= 1000) currentKPS++;
            else break;
        }
        if (currentKPS > m_peakKPS) m_peakKPS = currentKPS;
    }

    // 5. 左右手占比
    int leftHits = m_state.getLeftHandHits();
    int rightHits = m_state.getRightHandHits();
    int total = leftHits + rightHits;
    if (total > 0) {
        m_leftRatio = (double)leftHits / total;
        m_rightRatio = (double)rightHits / total;
    } else {
        m_leftRatio = m_rightRatio = 0.5;
    }
}

// ==========================================
// 极具冲击力的可视化图表
// ==========================================
void AnalysisWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. 画一个半透明的黑色遮罩面板，盖住底下的结算界面
    painter.fillRect(rect(), QColor(10, 15, 25, 230));

    int dx = (width() - 1600) / 2;
    int dy = (height() - 900) / 2;
    painter.translate(dx, dy);

    // 大标题
    painter.setPen(QColor(0, 255, 255));
    painter.setFont(QFont("Arial", 40, QFont::Bold));
    painter.drawText(150, 100, "MEOWTHM ANALYSIS REPORT");

    // 2. 绘制散点图 (x=150, y=180, w=700, h=300)
    drawScatterPlot(painter, QRect(150, 180, 800, 300));

    // 3. 绘制六维雷达图 (x=1050, y=300, w=400, h=400)
    drawRadarChart(painter, QRect(1050, 300, 400, 400));
}

void AnalysisWindow::drawScatterPlot(QPainter& painter, const QRect& rect)
{
    // 背景框与判定零线
    painter.setPen(QPen(QColor(255, 255, 255, 50), 2));
    painter.drawRect(rect);
    painter.setPen(QPen(QColor(0, 255, 255, 150), 2, Qt::DashLine));
    painter.drawLine(rect.left(), rect.center().y(), rect.right(), rect.center().y());

    // 刻度文字
    painter.setFont(QFont("Arial", 12));
    painter.drawText(rect.left() - 40, rect.top() + 15, "LATE");
    painter.drawText(rect.left() - 40, rect.bottom() - 5, "EARLY");

    QList<qint64> offsets = m_state.getHitOffsets();
    QList<qint64> times = m_state.getHitTimestamps();
    if (offsets.isEmpty()) return;

    qint64 minTime = times.first();
    qint64 maxTime = times.last();
    qint64 duration = maxTime - minTime;
    if (duration == 0) duration = 1;

    // 画散点 (发光圆点)
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < offsets.size(); ++i) {
        // X轴映射时间 (0 -> 100%)
        double xRatio = (double)(times[i] - minTime) / duration;
        int x = rect.left() + xRatio * rect.width();

        // Y轴映射误差 (-160 到 160) -> 映射到 rect.height()
        // offsets > 0 (Late) 画在上方，< 0 (Early) 画在下方
        double yRatio = (offsets[i] + 160.0) / 320.0;
        int y = rect.bottom() - yRatio * rect.height();

        // 根据准确度决定颜色 (越靠近0越准)
        int absOffset = std::abs(offsets[i]);
        if (absOffset <= 80) painter.setBrush(QColor(0, 255, 255, 200)); // Perfect: 蓝光
        else painter.setBrush(QColor(50, 205, 50, 200));                 // Good: 绿光

        painter.drawEllipse(QPoint(x, y), 3, 3);
    }
}

void AnalysisWindow::drawRadarChart(QPainter& painter, const QRect& rect)
{
    QPoint center = rect.center();
    double radius = rect.width() / 2.0 - 40;

    // 1. 画雷达底网 (六边形)
    painter.setPen(QPen(QColor(255, 255, 255, 100), 2));
    for (int step = 1; step <= 4; ++step) {
        QPolygonF poly;
        double r = radius * (step / 4.0);
        for (int i = 0; i < 6; ++i) {
            double angle = i * 60 * M_PI / 180.0 - M_PI / 2.0; // 从正上方开始
            poly << QPointF(center.x() + r * cos(angle), center.y() + r * sin(angle));
            if (step == 4) painter.drawLine(center, poly.last().toPoint()); // 画放射线
        }
        painter.drawPolygon(poly);
    }

    // 2. 准备数据并将其映射到 0.0 ~ 1.0 之间
    double vals[6];
    vals[0] = m_state.getCurrentAcc();                                      // ACC (精准)
    vals[1] = qMin(1.0, m_peakKPS / 15.0);                                  // 爆发 (15 KPS算满)
    vals[2] = qMax(0.0, 1.0 - m_stdDev / 80.0);                             // 稳定 (方差越小越满)
    vals[3] = m_leftRatio * 2.0 > 1.0 ? 1.0 : m_leftRatio * 2.0;            // 左脑协同
    vals[4] = m_rightRatio * 2.0 > 1.0 ? 1.0 : m_rightRatio * 2.0;          // 右脑协同
    vals[5] = qMax(0.0, 1.0 - std::abs(m_avgOffset) / 80.0);                // 神经偏离极小值

    // 3. 画玩家能力多边形 (发光紫粉色)
    QPolygonF statPoly;
    for (int i = 0; i < 6; ++i) {
        double angle = i * 60 * M_PI / 180.0 - M_PI / 2.0;
        double r = radius * vals[i];
        statPoly << QPointF(center.x() + r * cos(angle), center.y() + r * sin(angle));
    }

    painter.setPen(QPen(QColor(218, 112, 214, 255), 3));
    painter.setBrush(QColor(218, 112, 214, 100)); // 半透明填充
    painter.drawPolygon(statPoly);

    // 4. 文字标注
    QString labels[6] = {"ACCURACY", "PEAK KPS", "STABILITY", "L-BRAIN", "R-BRAIN", "REACT RATE"};
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    for (int i = 0; i < 6; ++i) {
        double angle = i * 60 * M_PI / 180.0 - M_PI / 2.0;
        int lx = center.x() + (radius + 20) * cos(angle);
        int ly = center.y() + (radius + 20) * sin(angle);
        painter.drawText(lx - 30, ly + 5, labels[i]);
    }
}

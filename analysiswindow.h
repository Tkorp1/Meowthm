#ifndef ANALYSISWINDOW_H
#define ANALYSISWINDOW_H

#include <QWidget>
#include <QPushButton>
#include "gamestate.h"

class AnalysisWindow : public QWidget
{
    Q_OBJECT
public:
    explicit AnalysisWindow(const GameState& state, QWidget *parent = nullptr);
    ~AnalysisWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void calculateMetrics(); // 核心计算引擎
    void drawRadarChart(QPainter& painter, const QRect& rect); // 画雷达图
    void drawScatterPlot(QPainter& painter, const QRect& rect); // 画散点图

    GameState m_state;
    QPushButton* m_closeBtn;

    // ----- 计算出的高级数据 -----
    double m_avgOffset;     // 平均偏差 (反应早晚)
    double m_stdDev;        // 标准差 (绝对稳定性)
    int m_peakKPS;          // 峰值手速 (每秒敲击次数)
    double m_leftRatio;     // 左侧发力占比
    double m_rightRatio;    // 右侧发力占比
};

#endif // ANALYSISWINDOW_H

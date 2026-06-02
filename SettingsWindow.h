#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

// 预览轨道控件：用于演示音符下落
class PreviewTrack : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewTrack(QWidget *parent = nullptr);
    ~PreviewTrack();

    // 设置下落速度（像素/毫秒）
    void setSpeed(double speed);

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;   // 使用定时器更新下落位置

private:
    double m_speed;          // 当前速度（像素/毫秒）
    double m_noteY;          // 音符的Y坐标（0~height）
    int m_timerId;           // 定时器ID
};

// 设置界面主窗口
class SettingsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

protected:
    void paintEvent(QPaintEvent *event) override;   // 绘制背景图

private slots:
    void onSpeedButtonClicked(double delta);        // 流速调节按钮（+0.05 / -0.05）
    void onOffsetButtonClicked(qint64 delta);       // 偏移调节按钮（+5 / -5）
    void onExitClicked();                           // 退出按钮

    void updateSpeedLabel(double speed);            // 更新流速显示标签
    void updateOffsetLabel(qint64 offset);          // 更新偏移显示标签

private:
    // UI控件
    QLabel *m_speedLabel;          // 显示当前流速
    QLabel *m_offsetLabel;         // 显示当前偏移值
    QPushButton *m_speedPlusBtn;   // 流速+0.05
    QPushButton *m_speedMinusBtn;  // 流速-0.05
    QPushButton *m_offsetPlusBtn;  // 偏移+5
    QPushButton *m_offsetMinusBtn; // 偏移-5
    QPushButton *m_exitBtn;        // 退出按钮

    // 预览轨道
    PreviewTrack *m_previewTrack;

    // 背景音乐
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
};

#endif // SETTINGSWINDOW_H

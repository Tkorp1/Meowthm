#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include <QLabel>
#include <QList>

// 【升级】：支持长条的数据结构
struct RecordNote {
    int type;      // 1 代表普通 Tap，2 代表长条 Hold
    int trackId;   // 轨道 0,1,2,3 (D,F,J,K)
    qint64 timeMs; // 起始时间戳
    qint64 endTime;// 结束时间戳 (Tap音符这个值通常填0或者等于timeMs)
};

class RecordWindow : public QWidget
{
    Q_OBJECT
public:
    explicit RecordWindow(QWidget *parent = nullptr);
    ~RecordWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override; // 【新增】松开按键的事件！
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onStartRecord();
    void onStopAndSave();

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;

    QList<RecordNote> m_recordedNotes;
    bool m_isRecording;

    // 【新增】：用来记录四个轨道当前是否被按下，以及按下的起始时间
    // 数组索引对应轨道号：0=D, 1=F, 2=J, 3=K
    // 如果值为 -1，代表当前轨道没被按下。
    qint64 m_pressTime[4];

    // UI 控件
    QPushButton *m_startBtn;
    QPushButton *m_saveBtn;
    QPushButton *m_backBtn;
    QLabel *m_statusLabel;
    QLabel *m_countLabel;
};

#endif // RECORDWINDOW_H

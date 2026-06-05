#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include <QLabel>
#include <QList>

// 我们用来存放录制数据的结构体
struct RecordNote {
    int trackId;   // 轨道 0,1,2,3 (D,F,J,K)
    qint64 timeMs; // 按下的毫秒时间戳
};

class RecordWindow : public QWidget
{
    Q_OBJECT
public:
    explicit RecordWindow(QWidget *parent = nullptr);
    ~RecordWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onStartRecord();
    void onStopAndSave();

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;

    QList<RecordNote> m_recordedNotes; // 存放录制的音符
    bool m_isRecording;                // 是否正在录制

    // UI 控件
    QPushButton *m_startBtn;
    QPushButton *m_saveBtn;
    QPushButton *m_backBtn;
    QLabel *m_statusLabel;
    QLabel *m_countLabel;
};

#endif // RECORDWINDOW_H

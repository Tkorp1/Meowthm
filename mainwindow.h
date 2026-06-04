#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QAudioOutput>
#include <QKeyEvent>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onSelectSong();
    /* 如果需要，可以取消下面这些槽函数的注释
    void onSettings();
    void onAchievements();
    void onPoke();
    void onExitGame();
    void onProfile();
    */

private:
    Ui::MainWindow *ui;
    QGraphicsView *view;
    QGraphicsScene *scene;

    // ======= 双缓冲区：无缝切换核心组件 =======
    QMediaPlayer *m_player1;
    QMediaPlayer *m_player2;
    QGraphicsVideoItem *m_videoItem1;
    QGraphicsVideoItem *m_videoItem2;
    QAudioOutput *m_audioOutput1;
    QAudioOutput *m_audioOutput2;

    bool m_isFirstVideo;
};

#endif // MAINWINDOW_H

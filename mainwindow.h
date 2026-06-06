#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QPixmap>

#include "scenemanager.h"

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
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void onSelectSong();

    Ui::MainWindow *ui;
    QLabel *titleLabel;

    // 精灵图动画变量
    QTimer *animTimer;
    QPixmap spriteSheet;
    int currentFrame;
    int totalFrames;
    int columns;
    int frameWidth;
    int frameHeight;
};

#endif // MAINWINDOW_H

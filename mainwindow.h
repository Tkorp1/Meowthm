#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSelectSong();      // 选曲界面
    void onSettings();        // 设置界面
    void onAchievements();    // 成就界面
    void onProfile();        // 个人档案
    void onPoke();            // 戳界面
    void onExitGame();        // 退出游戏

private:
    Ui::MainWindow *ui;
    QGraphicsView *view;
    QGraphicsScene *scene;
};

#endif // MAINWINDOW_H

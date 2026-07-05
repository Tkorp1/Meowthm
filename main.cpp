#include "mainwindow.h"
#include "scenemanager.h"
#include <QApplication>
#include <QStackedWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ==========================================
    // 1. 创建全局唯一的“主舞台”
    // ==========================================
    SceneManager::stage = new QStackedWidget();
    // 让舞台变成全屏、无边框的绝对底层
    SceneManager::stage->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    SceneManager::stage->showFullScreen();

    // ==========================================
    // 2. 召唤初始主菜单，并挂上舞台
    // ==========================================
    MainWindow *w = new MainWindow();
    SceneManager::switchScene(w);

    return a.exec();
}

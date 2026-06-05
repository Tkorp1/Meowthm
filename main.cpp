#include "mainwindow.h"
#include "gamescene.h"
#include "resultscene.h"
#include "SelectSongWindow.h" // 【新增】把选曲界面的头文件引进来
#include "recordwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    // ==========================================
    //  测试方案 A：直接看刚修好的【选曲界面】
    // ==========================================
    // SelectSongWindow* testSelect = new SelectSongWindow();
    // testSelect->show();


    // ==========================================
    // 测试方案 B：看最开始的【初始主菜单界面】
    // ==========================================
    // MainWindow *w = new MainWindow();
    // w->setAttribute(Qt::WA_DeleteOnClose); // 确保关闭时彻底释放内存
    // w->showFullScreen(); // 初始界面的全屏

    // return a.exec();


    // ==========================================
    // 测试方案 C：直接跳过UI，进入【打歌测试界面】
    // ==========================================
    // GameScene* testGame = new GameScene(":/map/maps/song1");
    // testGame->show();


    // ==========================================
    // 测试方案 D：直接看【结算界面】
    // ==========================================
    // GameState temp(":/map/maps/song1");
    // ResultScene* testresult = new ResultScene(temp);
    // testresult->show();


    // ==========================================
    // 测试方案 E：直接看【录制界面】
    // ==========================================
    RecordWindow* w = new RecordWindow();
    w -> show();



    return a.exec();
}

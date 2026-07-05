#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <QStackedWidget>
#include <QWidget>

class SceneManager
{
public:
    // 全局唯一的舞台容器
    static QStackedWidget* stage;

    // 统一的切换接口
    static void switchScene(QWidget* newScene);
};

#endif // SCENEMANAGER_H

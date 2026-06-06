#include "scenemanager.h"

QStackedWidget* SceneManager::stage = nullptr;

void SceneManager::switchScene(QWidget* newScene)
{
    if (!stage) return;

    // 拿到旧幕布
    QWidget* oldScene = stage->currentWidget();

    // 挂上新幕布并展示
    stage->addWidget(newScene);
    stage->setCurrentWidget(newScene);

    // 彻底销毁旧幕布，保持内存干净
    if (oldScene) {
        stage->removeWidget(oldScene);
        oldScene->deleteLater();
    }
}

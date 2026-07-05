#include "scenemanager.h"
#include <QResizeEvent>
#include <QWidget>
#include <QGridLayout> // 【新增】：引入网格布局来实现绝对居中
#include <QFrame>      // 【新增】：引入 QFrame 作为黑底容器

// ==========================================
// 纯手工定位
// ==========================================
class CenterWrapper : public QWidget {
public:
    QWidget* target;

    CenterWrapper(QWidget* c) : target(c) {
        // 1. 把新界面强制变成子控件
        target->setParent(this);

        // 2. 剥夺可能拥有的独立窗口权限，防止它试图跳出屏幕
        target->setWindowFlags(Qt::Widget);

        // 3. 开启强制背景绘制，并涂黑
        this->setAttribute(Qt::WA_StyledBackground, true);
        this->setObjectName("BlackWrapper");
        this->setStyleSheet("QWidget#BlackWrapper { background-color: black; }");

        // 4. 唤醒界面并转移键盘焦点，防止游戏丢键盘输入
        target->show();
        target->setFocus();
    }

protected:
    // 只要屏幕尺寸一变，就重新计算中心
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        if (target) {
            // 纯手工算出居中的 X 和 Y 坐标
            int x = (this->width() - target->width()) / 2;
            int y = (this->height() - target->height()) / 2;
            target->move(x, y);
        }
    }
};

QStackedWidget* SceneManager::stage = nullptr;

void SceneManager::switchScene(QWidget* newScene)
{
    if (!stage) return;

    QWidget* oldScene = stage->currentWidget();

    // 把你的新场景塞进去
    CenterWrapper* wrapper = new CenterWrapper(newScene);

    stage->addWidget(wrapper);
    stage->setCurrentWidget(wrapper);

    // 内存清理机制不变
    if (oldScene) {
        stage->removeWidget(oldScene);
        oldScene->deleteLater();
    }
}

#include "SelectSongWindow.h"
#include "SettingsWindow.h"
#include "PokeWindow.h"
#include "ProfileWindow.h"
#include "AchievementsWindow.h"
#include "MainWindow.h"
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <functional>

SelectSongWindow::SelectSongWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("选曲界面");
    setFixedSize(1200, 800);

    // 按钮样式：完全透明，无文字，无边框
    QString buttonStyle = R"(
        QPushButton {
            background-color: blue;
            color: transparent;
            border: none;
        }
        QPushButton:hover {
            background-color: transparent;
        }
        QPushButton:pressed {
            background-color: transparent;
        }
    )";

    // 按钮大小（你可以自行调整）
    int btnWidth = 200;
    int btnHeight = 110;

    // 定义按钮数据：文字、x坐标、y坐标、槽函数
    struct ButtonData {
        QString text;
        int x;
        int y;
        std::function<void()> onClick;
    };

    QList<ButtonData> buttons = {
        { "设置", 361, 498, [this]() { onSettings(); } },
        { "戳", 727, 336, [this]() { onPoke(); } },
        { "个人档案", 625, 498, [this]() { onProfile(); } },
        { "成就", 880, 498, [this]() { onAchievements(); } },
        { "返回主界面", 1025, 24, [this]() { onBackToMain(); } }
    };

    for (const auto &data : buttons) {
        QPushButton *btn = new QPushButton(data.text, this);
        btn->setFixedSize(btnWidth, btnHeight);
        btn->setStyleSheet(buttonStyle);
        btn->move(data.x, data.y);
        QObject::connect(btn, &QPushButton::clicked, data.onClick);
    }
}

SelectSongWindow::~SelectSongWindow() {}

void SelectSongWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // 加载背景图（请将 bg_select.png 放在工作目录，或修改路径）
    QPixmap bg("bg_select.png");
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), QColor(30, 30, 40));
        qDebug() << "背景图 bg_select.png 未找到，使用纯色背景";
    }
    QWidget::paintEvent(event);
}

void SelectSongWindow::onSettings()
{
    SettingsWindow *window = new SettingsWindow(this);
    window->show();
}

void SelectSongWindow::onPoke()
{
    PokeWindow *window = new PokeWindow(this);
    window->show();
}

void SelectSongWindow::onProfile()
{
    ProfileWindow *window = new ProfileWindow(this);
    window->show();
}

void SelectSongWindow::onAchievements()
{
    AchievementsWindow *window = new AchievementsWindow(this);
    window->show();
}

void SelectSongWindow::onBackToMain()
{
    // 关闭当前选曲窗口，并确保主窗口显示
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

#ifndef RESULTSCENE_H
#define RESULTSCENE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPixmap>
#include "gamestate.h"
#include "gameconfig.h"
#include "gamescene.h"

class ResultScene : public QWidget
{
    Q_OBJECT

private:
    // 1.成绩数据
    GameState state;

    // 2.UI控件
    QLabel* titleLabel;

    QLabel* songNameLabel;

    QLabel* scoreLabel;

    QLabel* accLabel;

    QLabel* comboLabel;

    QLabel* perfectLabel;

    QLabel* goodLabel;

    QLabel* missLabel;

    QPushButton* returnButton;

private slots:
    // 返回主菜单
    void onReturnMainMenu();

public:
    explicit ResultScene(const GameState& _state,
                         QWidget *parent = nullptr);

    ~ResultScene();

protected:

    void paintEvent(QPaintEvent *event)override;
};

#endif // RESULTSCENE_H

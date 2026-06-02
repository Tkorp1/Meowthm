#ifndef RESULTSCENE_H
#define RESULTSCENE_H

#include <QWidget>
#include "gamestate.h"

class ResultScene : public QWidget
{
    Q_OBJECT

private:
    GameState state;

public:
    // 注意这里结束界面使用state来初始化
    explicit ResultScene(GameState _state, QWidget *parent = nullptr);

signals:
};

#endif // RESULTSCENE_H

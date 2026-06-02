#include "resultscene.h"

ResultScene::ResultScene(GameState _state, QWidget *parent)
    : QWidget{parent}
{
    state = _state;
}

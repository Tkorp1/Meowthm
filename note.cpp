#include "note.h"

Note::Note(qint64 _targetTime, int _noteWidth, int _noteHeight, NoteType _type, QWidget* parent)
    :QLabel(parent),targetTime(_targetTime),type(_type){

    this->setFixedSize(_noteWidth, _noteHeight);

    // 最简单的皮肤
    if (type == TAP) {
        this->setStyleSheet("background-color: #00BFFF; border-radius: 5px;"); // 蓝色
    } else {
        this->setStyleSheet("background-color: #32CD32;"); // 绿色
    }
}

Note::~Note(){

}

qint64 Note::getTargetTime()const{
    return targetTime;
}

int Note::getCurrentY()const{
    return this->y();
}

NoteType Note::getType()const{
    return type;
}

void Note::updateY(int newY){
    this->move(this->x(), newY);

    return;
}

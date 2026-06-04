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

// 所有的get函数：

qint64 Note::getTargetTime()const{
    return targetTime;
}

int Note::getCurrentY()const{
    return this->y();
}

NoteType Note::getType()const{
    return type;
}

// 其他的函数：

void Note::updateY(int newY){
    this->move(this->x(), newY);

    return;
}

void Note::changeTargetTime(qint64 ti){
    targetTime += ti;
    return;
}

Tap::Tap(qint64 _targetTime, int _noteWidth, int _noteHeight, QWidget* parent)
    :Note(_targetTime, _noteWidth, _noteHeight, NoteType::TAP, parent){

}

Tap::~Tap(){

}


Hold::Hold(qint64 _targetTime, qint64 _tailTime, int _noteWidth, int _noteHeight, QWidget* parent)
    :Note(_targetTime, _noteWidth, _noteHeight, NoteType::HOLD, parent), tailTime(_tailTime){

}

Hold::~Hold(){

}

qint64 Hold::getTailTime() const{
    return tailTime;
}

void Hold::changeTargetTime(qint64 ti){
    targetTime += ti;
    tailTime += ti;
    return;
}
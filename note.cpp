#include "note.h"

Note::Note(qint64 _targetTime, int _noteWidth, int _noteHeight, NoteType _type, QWidget* parent)
    :QLabel(parent),targetTime(_targetTime),type(_type){

    this->setFixedSize(_noteWidth, _noteHeight);

}


void Note::setNoteColor(int r, int g, int b) {
    m_r = r; m_g = g; m_b = b;
    if (type == TAP) {
        // Tap 完全不透明，带圆角
        this->setStyleSheet(QString("background-color: rgb(%1, %2, %3); border-radius: 5px;").arg(r).arg(g).arg(b));
    } else {
        // Hold 稍微透明 (透明度 200/255)
        this->setStyleSheet(QString("background-color: rgba(%1, %2, %3, 200);").arg(r).arg(g).arg(b));
    }
}

// Hold 断掉
void Note::setMissed() {
    this->setStyleSheet(QString("background-color: rgba(%1, %2, %3, 50);").arg(m_r).arg(m_g).arg(m_b));
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

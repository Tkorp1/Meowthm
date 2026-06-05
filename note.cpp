#include "note.h"
#include <QPainter>
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
        // Hold的逻辑改变
        update();
    }
}

// Hold 断掉
void Note::setMissed() {
    this->setStyleSheet(QString("background-color: rgba(%1, %2, %3, 60);").arg(m_r).arg(m_g).arg(m_b));
}

QColor Note::getHeadColor() const {

    return QColor(m_r,m_g,m_b);

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

// TAP

Tap::Tap(qint64 _targetTime, int _noteWidth, int _noteHeight, QWidget* parent)
    :Note(_targetTime, _noteWidth, _noteHeight, NoteType::TAP, parent){

}

Tap::~Tap(){

}

// HOLD

Hold::Hold(qint64 _targetTime, qint64 _tailTime, int _noteWidth, int _noteHeight, QWidget* parent)
    :Note(_targetTime, _noteWidth, _noteHeight, NoteType::HOLD, parent), tailTime(_tailTime){

}

Hold::~Hold(){

}

qint64 Hold::getTailTime() const{
    return tailTime;
}

void Hold::setState(HoldState state) {
    m_state = state;
    update();
}

void Hold::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const int headHeight = 20;
    // 1. body（尾巴）
    painter.setPen(Qt::NoPen);
    QColor bodyColor;
    if(m_state == HOLD_PRESSED){
        bodyColor = m_tailPressedColor;
    }
    else if(m_state == HOLD_MISS){
        bodyColor = QColor(m_r, m_g, m_b, 60);
    }
    else{
        bodyColor = QColor(m_r, m_g, m_b, 120);
    }
    painter.setBrush(bodyColor);
    painter.drawRect(
        0,
        0,
        width(),
        height() - headHeight
        );
    // 2. head（tap部分）
    painter.setBrush(getHeadColor());
    painter.drawRoundedRect(
        0,
        height() - headHeight,
        width(),
        headHeight,
        5,
        5
        );
}
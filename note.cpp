#include "note.h"

Note::Note(qint64 _targetTime, int _noteWidth, int _noteHeight, NoteType _type, QWidget* parent)
    :QLabel(parent),targetTime(_targetTime),type(_type),noteWidth(_noteWidth),noteHeight(_noteHeight){}

Note::~Note(){

}

qint64 Note::getTargetTime()const{
    return targetTime;
}

int Note::getCurrentY()const{
    return currentY;
}

NoteType Note::getType()const{
    return type;
}

void Note::updateY(int newY){
    currentY=newY;
    return;
}
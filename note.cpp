#include "note.h"

Note::Note(qint64 _targetTime, NoteType _type, QWidget* parent)
    :QLabel(parent),targetTime(_targetTime),type(_type){}

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
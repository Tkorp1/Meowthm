#include "track.h"
#include "note.h"

Track::Track(int _trackIndex, int _hitLineY, int _xPos, QObject* parent):
    QObject(parent),trackIndex(_trackIndex),xPos(_xPos),hitLineY(_hitLineY)
{

}

Track::~Track(){}

void Track::addNotes(const QList<Note*> noteList){
    noteInTrack=noteList;
}

void Track::updateTrack(qint64 currentMusicTime, double currentSpeed){

}

void Track::setNoteParent(QWidget* parent){
    for(Note* note : std::as_const(noteInTrack)){
        note->setParent(parent);
    }
}

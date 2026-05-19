#include "track.h"
#include "note.h"

Track::Track() {}

void Track::setNoteParent(QWidget* parent){
    for(Note* note : noteInTrack){
        note->setParent(parent);
    }
}

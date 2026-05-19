#include "track.h"
#include "note.h"

// 0.构造函数
Track::Track(int _trackIndex, int _hitLineY, int _xPos, QObject* parent):
    QObject(parent),trackIndex(_trackIndex),xPos(_xPos),hitLineY(_hitLineY)
{

}

// 1.析构函数
Track::~Track(){}

// 2.添加音符的函数,输入note的list
void Track::addNotes(const QList<Note*> noteList){
    noteInTrack=noteList;
}

// 3.更新轨道音符，输入当前绝对时间，同时负责清理miss的音符和判定hold结束
// Y = hitLineY - detaT * speed
// 在这个函数里要调用 note 的update函数更改 note 的y坐标，同时要删除已经 miss 的音符，在miss时报告信号 noteJudged(1)
// 在引入 hold 之后，需要增加 hold 是否完结的判断，即 currentMusicTime >= currentHoldingNote->getTailTime()
void Track::updateTrack(qint64 currentMusicTime, double currentSpeed){
    for(int i = noteInTrack.size() - 1; i >= 0; ++i){
        double newY = hitLineY - currentSpeed * (noteInTrack[i]->getTargetTime() - currentMusicTime);
        noteInTrack[i]->updateY(newY);
        // 下面判断这个音符是否需要被删除并且发出miss的信号：
        if(currentMusicTime - noteInTrack[i]->getTargetTime() > 160){
            // 现在这个音符已经出了miss的范围，需要删除
            emit noteJudged(1);
            delete noteInTrack.takeAt(i);
        }
    }
}

// 4.向gamescene提供轨道上现存的所有音符（miss的已经被update删掉了）,返回引用
const QList<Note*>& Track::getCurrentNote() const{
    return noteInTrack;
}

// 5.主动按键判定接口，接受gamescene捕捉的键盘输入
// 具体逻辑:拿出第一个 note 计算与 targetTime 的时间差，删除已经判定的 note ，调用 信号noteJudged 给 GameScene 加分
// 返回值建议 0=空, 1=Miss（hold的松手）, 2=Good, 3=Perfect
/* 注意在引入hold之后,当该 note 是 hold 并且已经打出 good 或者 perfect
     * ，需要将这个 hold 赋值给 currentHoldingNote，进入hold的后续判定 */
void  Track::checkHit(qint64 currentMusicTime){
    // 首先判断轨道是不是空的，如果没有音符了就返回
    if(noteInTrack.isEmpty()){
        return;
    }

    // 现在可以开始判定了
    Note* note = noteInTrack.front();
    qint64 deltaT = currentMusicTime - note->getTargetTime();
    deltaT = qAbs(deltaT);
    // 获得时间差并且取绝对值
    if(deltaT > 160){
        // 无事发生，没有按到
        emit noteJudged(0);
    }
    else if(deltaT <= 160 && deltaT > 80){
        // 现在是good
        emit noteJudged(2);
        delete noteInTrack.takeFirst();
    }
    else if(deltaT <= 80){
        // 现在是perfect判定
        emit noteJudged(3);
        delete noteInTrack.takeFirst();
    }
}


//下面是 hold 的逻辑可以先不写
// 6.对hold的长按释放判定
/* 当此时 currentHoldingNote 不为 nullptr 且松开按键，说明玩家松手了，触发miss并 delete 这个音符，
     指针悬空*/
// 如果提前松手导致 Miss，返回 true，否则返回 false
bool Track::isReleased(qint64 currentMusicTime){
    // 留给以后的自己
}


// 7.获取track的id的函数
int Track::getTrackId()const{
    return trackIndex;
}

// 8.设置note的父类指针
void Track::setNoteParent(QWidget* parent){
    for(Note* note : std::as_const(noteInTrack)){
        note->setParent(parent);
    }
}

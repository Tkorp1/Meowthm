#include "track.h"
#include "note.h"

// 0.构造函数
Track::Track(int _trackIndex, int _hitLineY, int _xPos, QObject* parent):
    QObject(parent),trackIndex(_trackIndex),xPos(_xPos),hitLineY(_hitLineY)
{

}

// 1.析构函数
Track::~Track(){
    while(!noteInTrack.isEmpty()){
        delete noteInTrack.takeFirst();
    }
}

// 2.添加音符的函数,输入note的list
void Track::addNotes(const QList<Note*>& noteList){
    noteInTrack=noteList;
}

// 3.更新轨道音符，输入当前绝对时间，同时负责清理miss的音符和判定hold结束
// Y = hitLineY - detaT * speed
// 在这个函数里要调用 note 的update函数更改 note 的y坐标，同时要删除已经 miss 的音符，在miss时报告信号 noteJudged(1)
// 在引入 hold 之后，需要增加 hold 是否完结的判断，即 currentMusicTime >= currentHoldingNote->getTailTime()
void Track::updateTrack(qint64 currentMusicTime, double currentSpeed){
    for(int i = noteInTrack.size() - 1; i >= 0; --i){
        double newY = hitLineY - currentSpeed * (noteInTrack[i]->getTargetTime() - currentMusicTime);
        noteInTrack[i]->updateY(newY - noteInTrack[i]->height());

        // 下面判断这个音符是否需要被删除并且发出miss的信号：
        if(currentMusicTime > noteInTrack[i]->getTargetTime() &&
           (currentMusicTime - noteInTrack[i]->getTargetTime() > 160)){
            // 现在这个音符已经出了miss的范围，需要删除
            emit noteJudged(1);
            if (noteInTrack[i]->getType() == NoteType::HOLD) {
                Note* missedHold = noteInTrack.takeAt(i);
                missedHold->setMissed(); // 调用你刚写的虚化函数

                // 之前有残留的尸体？先清理掉防止内存泄漏
                if (currentHoldingNote != nullptr) delete currentHoldingNote;

                // 移交给 currentHoldingNote 让它作为“尸体”继续掉落
                currentHoldingNote = missedHold;
                currentHoldHeadResult = 1; // 标记 1 代表它是尸体，别再给它发分了
            } else {
                // Tap 音符漏掉了，没救了，直接火化 (delete)
                delete noteInTrack.takeAt(i);
            }
        }
    }

    // 更新当前正在长按的Hold
    if (currentHoldingNote != nullptr) {
        // 1. 继续掉落
        double holdY = hitLineY - currentSpeed * (currentHoldingNote->getTargetTime() - currentMusicTime);
        currentHoldingNote->updateY(holdY - currentHoldingNote->height());

        // 2. 检查是否按到结尾
        if (currentMusicTime >= dynamic_cast<Hold*>(currentHoldingNote)->getTailTime()) {
            if (currentHoldHeadResult != 1) {
                emit noteJudged(currentHoldHeadResult);
            }
            // 【核心】：不管是活着的长条还是尸体，掉到底了就彻底 delete，释放内存！
            delete currentHoldingNote;
            currentHoldingNote = nullptr; // 安全清空
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
    // 获得时间差
    if(deltaT > 160 || deltaT < -160){
        // 无事发生，没有按到
        emit noteJudged(0);
    }
    else if((deltaT <= 160 && deltaT > 80)||(deltaT >= -160 && deltaT < -80)){
        // 现在是good

        // 判断是不是 Hold
        if (note->getType() == NoteType::HOLD) {
            if (currentHoldingNote != nullptr) delete currentHoldingNote;

            currentHoldingNote = note; // 挂载到专属指针
            currentHoldHeadResult = 2;
            noteInTrack.takeFirst();   // 从队列移除，但不delete！！！
        } else {
            emit noteJudged(2); // 普通的Tap立刻加分
            delete noteInTrack.takeFirst();
        }
    }
    else if(deltaT <= 80 && deltaT >= -80){
        // 现在是perfect判定
        if (note->getType() == NoteType::HOLD) {
            if (currentHoldingNote != nullptr) delete currentHoldingNote;
            currentHoldingNote = note;
            currentHoldHeadResult = 3;
            noteInTrack.takeFirst();
        } else {
            emit noteJudged(3);
            delete noteInTrack.takeFirst();
        }
    }
}


//下面是 hold 的逻辑可以先不写
// 6.对hold的长按释放判定
/* 当此时 currentHoldingNote 不为 nullptr 且松开按键，说明玩家松手了，触发miss并 delete 这个音符，
     指针悬空*/
// 如果提前松手导致 Miss，返回 true，否则返回 false
bool Track::isReleased(qint64 currentMusicTime){
    if (currentHoldingNote == nullptr) {
        return false;
    }

    qint64 timeUntilEnd = dynamic_cast<Hold*>(currentHoldingNote)->getTailTime() - currentMusicTime;
    qint64 totalDuration = dynamic_cast<Hold*>(currentHoldingNote)->getTailTime() - dynamic_cast<Hold*>(currentHoldingNote)->getTargetTime();
    if (timeUntilEnd > totalDuration * 0.4) {
        // 提前超过60%松手，miss
        emit noteJudged(1); // 触发 Miss，打断 Combo！
        currentHoldingNote->setMissed();
        currentHoldHeadResult = 1; // 标记为尸体
        return true;
    } else {
        // 发放头判分数！
        emit noteJudged(currentHoldHeadResult);
        delete currentHoldingNote;
        currentHoldingNote = nullptr;
        return false;
    }
}


// 7.获取track的id的函数
int Track::getTrackId()const{
    return trackIndex;
}

// 8.设置note的父类指针
void Track::setNoteParent(QWidget* parent){
    for(Note* note : std::as_const(noteInTrack)){
        note->setParent(parent);
        note->changeTargetTime(2000);

        // 这里把音符横向移动到它的轨道上
        // 把初始Y放到屏幕外面（-100），等主循环来接管它的掉落
        note->move(this->xPos, -100);
        note->show();

    }
}

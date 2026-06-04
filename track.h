#ifndef TRACK_H
#define TRACK_H

#include <QObject>
#include <QList> // note用list储存，便于删除头元素和访问
#include "note.h"

class Track : public QObject
{
    Q_OBJECT

protected:
    // 所有的成员变量：

    // 1.轨道编号 0 1 2 3
    int trackIndex;

    // 2.存放所有存活的音符实例，数据类型为list
    QList<Note*> noteInTrack;

    // 3.轨道位置（横坐标，长度和其他的参数在GameScene里面，是全局的数据
    int xPos;

    // 4.判定纵坐标高度
    int hitLineY;


    //下面是关于hold的逻辑，可以暂时先不写
    // 5.记录当前正在长按的 hold 音符
    Note* currentHoldingNote = nullptr;

    int currentHoldHeadResult = 0;



public:
    // 0.构造函数
    explicit Track(int _trackIndex, int _hitLineY, int _xPos, QObject* parent = nullptr);

    // 1.析构函数
    ~Track();

    // 2.添加音符的函数,输入note的list
    void addNotes(const QList<Note*>& noteList);

    // 3.更新轨道音符，输入当前绝对时间，同时负责清理miss的音符和判定hold结束
    // Y = hitLineY - detaT * speed
    // 在这个函数里要调用 note 的update函数更改 note 的y坐标，同时要删除已经 miss 的音符，在miss时报告信号 noteJudged(1)
    // 在引入 hold 之后，需要增加 hold 是否完结的判断，即 currentMusicTime >= currentHoldingNote->getTailTime()
    void updateTrack(qint64 currentMusicTime, double currentSpeed);


    // 4.向gamescene提供轨道上现存的所有音符（miss的已经被update删掉了）,返回引用
    const QList<Note*>& getCurrentNote() const;

    // 5.主动按键判定接口，接受gamescene捕捉的键盘输入
    // 具体逻辑:拿出第一个 note 计算与 targetTime 的时间差，删除已经判定的 note ，调用 信号noteJudged 给 GameScene 加分
    // 返回值建议 0=空, 1=Miss（hold的松手）, 2=Good, 3=Perfect
    /* 注意在引入hold之后,当该 note 是 hold 并且已经打出 good 或者 perfect
     * ，需要将这个 hold 赋值给 currentHoldingNote，进入hold的后续判定 */
    void  checkHit(qint64 currentMusicTime);



    //下面是 hold 的逻辑可以先不写
    // 6.对hold的长按释放判定
    /* 当此时 currentHoldingNote 不为 nullptr 且松开按键，说明玩家松手了，触发miss并 delete 这个音符，
     指针悬空*/
    // 如果提前松手导致 Miss，返回 true，否则返回 false
    bool isReleased(qint64 currentMusicTime);

    // 7.获取track的id的函数
    int getTrackId()const;

    // 8.设置note的父类指针，同时为初始的时候增加两秒的时间
    void setNoteParent(QWidget* parent);

signals:
    // 9.向gamescene报告判定的信号，只需要声明不需要实现
    // 返回值建议 0=空, 1=Miss（hold的松手）, 2=Good, 3=Perfect
    void noteJudged(int result);


};

#endif // TRACK_H

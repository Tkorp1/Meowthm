#ifndef TRACK_H
#define TRACK_H

#include <QObject>
#include <QList> // note用list储存，便于删除头元素和访问
#include "note.h"

class Track : public QObject
{
    Q_OBJECT
public:
    // 0.构造函数
    Track(int _trackIndex, int _hitLineY, int _xPos);

    // 1.析构函数
    ~Track();

    // 2.添加音符的函数,输入指向note的指针
    void addNote(Note* note);

    // 3.更新轨道音符，输入当前绝对时间
    // 在这个函数里要调用 note 的update函数更改 note 的y坐标，同时要删除已经 miss 的音符
    void updateTrack(qint64 currentMusicTime);


    // 4.向gamescene提供轨道上现存的所有音符（miss的已经被update删掉了）
    const QVector<Note*> getCurrentNote(qint64 currentMusicTime);

    // 5.判定接口，接受gamescene捕捉的键盘输入
    // 注意在引入hold之后要在里面单独分出来hold
    void checkHit(qint64 currentMusicTime);

    // 6.对hold的长按释放接口判定（使用方法目前存疑
    void checkRelease(qint64 currentMusicTime);

protected:
    // 所有的成员变量：

    // 1.轨道编号 0 1 2 3
    int trackIndex;

    // 2.存放所有存活的音符实例，数据类型为list
    QList<Note*> noteInTrack;

    // 3.轨道位置（横坐标，长度和其他的参数在GameScene里面，是全局的数据
    int xPOs;

    // 4.判定纵坐标高度
    int hitLineY;

    //4.QWidget
    QWidget* trackParent;

};

#endif // TRACK_H

#ifndef TRACK_H
#define TRACK_H

#include <QObject>
#include <QVector>

class Note;

class Track : public QObject
{
    Q_OBJECT
public:
    Track();
    //1.构造函数，使用explicit

    //！！！！这里还没写
    explicit Track();

    //2.添加音符的函数（自动重定向到相关文件然后读取，不需要参数（？
    void addNote();

    //3.根据流速和当前时间给gameScene需要画在当前图中间的音符
    QVector<Note*> getCurrentNote(double noteSpeed, qint64 currentTime);

    //4.更新时间：
    void updateTime(qint64 _time);

    //所有的获取函数：


protected:
    //所有的成员变量：

    //1.轨道编号（从0开始）
    int tracIndex;

    //2.音符vector
    QVector<Note*> noteInTrack;

    //3.轨道位置（横坐标，长度和其他的参数在GameScene里面，是全局的数据
    int xPOs;

    //4.QWidget
    QWidget* trackParent;

};

#endif // TRACK_H

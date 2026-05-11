#ifndef NOTE_H
#define NOTE_H

#include <QLabel>
#include <QObject>

// 定义音符枚举
enum NoteType{TAP, HOLD};

// note是音符基类，继承自QLabel，后续添加贴图
class Note : public QLabel
{
    Q_OBJECT

protected:
    // 0.判定时间戳
    qint64 targetTime;

    // 1.当前y坐标
    int currentY;

    // 2.音符类型
    NoteType type;


public:

    // 0.构造函数
    // 注意:该构造函数使用了 explicit 关键字，不能进行隐式类型转换！！！
    explicit Note(qint64 _targetTime, NoteType _type = TAP, QWidget* parent = nullptr);

    // 1.析构函数
    virtual ~Note();

    // 2.获取成员变量并无法修改，保证封装
    qint64 getTargetTime() const;
    int getCurrentY() const;
    NoteType getType() const;

    // 3.核心接口：移动函数，改变y坐标
    void updateY(int newY);


};

#endif // NOTE_H

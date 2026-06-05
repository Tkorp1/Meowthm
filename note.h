#ifndef NOTE_H
#define NOTE_H

#include <QLabel>
#include <QObject>

// 定义音符枚举
enum NoteType{TAP, HOLD};

// note是音符基类，继承自QLabel，后续添加贴图
// 对于note的显示，“哪里new出来哪里就负责show”，所以note的显示在谱面解析器里面！！！
class Note : public QLabel
{
    Q_OBJECT

protected:
    // 0.判定时间戳
    qint64 targetTime;

    // 1.音符类型
    NoteType type;

    // 2.颜色
    int m_r, m_g, m_b;


public:

    // 0.构造函数
    // 注意:该构造函数使用了 explicit 关键字，不能进行隐式类型转换！！！
    explicit Note(qint64 _targetTime, int _noteWidth, int _noteHeight, NoteType _type = TAP, QWidget* parent = nullptr);

    // 1.析构函数
    virtual ~Note();

    // 2.获取成员变量并无法修改，保证封装
    qint64 getTargetTime() const;
    int getCurrentY() const;
    NoteType getType() const;

    // 3.核心接口：移动函数，改变y坐标
    void updateY(int newY);

    // 4.新增，修改时间函数
    virtual void changeTargetTime(qint64 ti);

    // 5.美化
    void setNoteColor(int r, int g, int b);
    void setMissed();

};


class Tap: public Note{
public:
    // 去掉了_type参数
    explicit Tap(qint64 _targetTime, int _noteWidth, int _noteHeight, QWidget* parent = nullptr);
    virtual ~Tap();
};


class Hold: public Note{
    Q_OBJECT

protected:
    // Hold独有的数据：尾部判定时间
    qint64 tailTime;

public:
    // 注意：相比 Tap，多了一个 _tailTime 参数，同样去掉了 _type 参数
    // 注意，hold的 noteHeight 是需要创建时根据时间和流速实时计算的
    explicit Hold(qint64 _targetTime, qint64 _tailTime, int _noteWidth, int _noteHeight, QWidget* parent = nullptr);

    virtual ~Hold();

    // 检查是否结束的接口
    qint64 getTailTime() const;


};

#endif // NOTE_H

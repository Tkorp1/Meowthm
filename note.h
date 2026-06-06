#ifndef NOTE_H
#define NOTE_H

#include <QLabel>
#include <QObject>
#include <QColor>

// 定义音符枚举
enum NoteType{TAP, HOLD};

// hold的状态
enum HoldState {
    HOLD_NORMAL,      // 未判定
    HOLD_PRESSED,      // 已命中头判，正在按
    HOLD_MISS,        // 头判miss
    HOLD_FINISHED     // 已完成
};

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
    int m_r = 255, m_g = 255, m_b = 255;

    static constexpr int HOLD_HEAD_HEIGHT = 20;


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


    // 6.颜色函数
    QColor getHeadColor() const;
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

    HoldState m_state = HOLD_NORMAL;

    QColor m_tailNormalColor;

    QColor m_tailPressedColor;
    // Hold画图特殊：
    void paintEvent(QPaintEvent* event) override;

    //是否被吸附
    bool m_showAttachedHead = false;

public:
    // 注意：相比 Tap，多了一个 _tailTime 参数，同样去掉了 _type 参数
    // 注意，hold的 noteHeight 是需要创建时根据时间和流速实时计算的
    explicit Hold(qint64 _targetTime, qint64 _tailTime, int _noteWidth, int _noteHeight, QWidget* parent = nullptr);

    virtual ~Hold();

    // 检查是否结束的接口
    qint64 getTailTime() const;

    // 画图相关函数
    void setState(HoldState state);

    HoldState getState() const;

    void setTailColors(const QColor& normal, const QColor& pressed);

    void setAttachedHead(bool value);

    bool getAttachedHead() const;

};

#endif // NOTE_H

#ifndef BUTTONITEM_H
#define BUTTONITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class ButtonItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    // 构造函数：传入三种状态的图片路径（普通、悬停、按下）
    ButtonItem(const QString &normalImg,
               const QString &hoverImg = QString(),
               const QString &pressedImg = QString(),
               QGraphicsItem *parent = nullptr);

    // 重写鼠标事件实现交互
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

signals:
    void clicked();   // 点击信号

private:
    QString m_normalImgPath;
    QString m_hoverImgPath;
    QString m_pressedImgPath;
    bool m_isPressed;   // 标记是否按下中
};

#endif // BUTTONITEM_H

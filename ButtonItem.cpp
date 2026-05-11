#include "ButtonItem.h"
#include <QPixmap>

ButtonItem::ButtonItem(const QString &normalImg,
                       const QString &hoverImg,
                       const QString &pressedImg,
                       QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , m_normalImgPath(normalImg)
    , m_hoverImgPath(hoverImg.isEmpty() ? normalImg : hoverImg)   // 如果未提供悬停图，就用普通图
    , m_pressedImgPath(pressedImg.isEmpty() ? normalImg : pressedImg) // 如果未提供按下图，也用普通图
    , m_isPressed(false)
{
    // 加载并设置普通状态的图片
    QPixmap pixmap(m_normalImgPath);
    setPixmap(pixmap);

    // 允许接受悬停事件（实现 hover 效果）
    setAcceptHoverEvents(true);
}

void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        // 切换到按下状态的图片
        QPixmap pixmap(m_pressedImgPath);
        setPixmap(pixmap);
    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void ButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isPressed && event->button() == Qt::LeftButton) {
        m_isPressed = false;
        // 恢复为普通状态（如果鼠标还在按钮区域内，则恢复为悬停图；否则恢复普通图）
        if (contains(event->pos())) {
            QPixmap pixmap(m_hoverImgPath);
            setPixmap(pixmap);
        } else {
            QPixmap pixmap(m_normalImgPath);
            setPixmap(pixmap);
        }
        // 发射点击信号
        emit clicked();
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void ButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // 鼠标进入时，若不是按下状态，则显示悬停图片
    if (!m_isPressed) {
        QPixmap pixmap(m_hoverImgPath);
        setPixmap(pixmap);
    }
    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void ButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    // 鼠标离开时，若不是按下状态，则显示普通图片
    if (!m_isPressed) {
        QPixmap pixmap(m_normalImgPath);
        setPixmap(pixmap);
    }
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}

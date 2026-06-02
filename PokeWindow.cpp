// PokeWindow.cpp
#include "PokeWindow.h"
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QDebug>
#include <QMouseEvent>
//  调试用显示鼠标位置
/*void PokeWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    qDebug() << "鼠标点击坐标:" << pos.x() << pos.y();
    QWidget::mousePressEvent(event);
}*/

PokeWindow::PokeWindow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(1200, 800);
    setWindowTitle("戳");

    // 固定文本1
    m_fixedText1 = new QTextEdit(this);
    m_fixedText1->setGeometry(145, 170, 200, 350);
    m_fixedText1->setPlainText("你说得对，但‌Meowthm是由三个蛋自主研发的一款守旧二维不立体节奏游戏。‌游戏发生在一个被称作（听不懂喵）的幻想世界，"
                               "在这里，玩家将扮演一只可爱小猫，‌在自由的交流中邂逅性格各异、‌能力独特的note们，‌共同击败这下看懂了，‌找回失散的蛋，‌同时逐步发掘滚木的真相。‌‌");
    m_fixedText1->setReadOnly(true);
    m_fixedText1->setStyleSheet("background-color:transparent;color:black; font-size: 18px;");

    // 固定文本2
    m_fixedText2 = new QTextEdit(this);
    m_fixedText2->setGeometry(508, 376, 500, 240);
    m_fixedText2->setPlainText("这是第二个固定内容框。\n也可以放提示信息。");
    m_fixedText2->setReadOnly(true);
    m_fixedText2->setStyleSheet("background-color: transparent; font-size: 14px;");

    // 返回按钮
    m_backBtn = new QPushButton("", this);
    m_backBtn->setGeometry(50, 730, 75, 50);
    m_backBtn->setStyleSheet("background-color:transparent; color: white; font-size: 16px; border-radius: 8px;");
    connect(m_backBtn, &QPushButton::clicked, this, &PokeWindow::onBackClicked);

    // 动态图片和文本框（位置、大小后续可根据背景图调整）
    m_dynamicImage = new QLabel(this);
    m_dynamicImage->setGeometry(492, 116, 200, 250);
    m_dynamicImage->setScaledContents(true);  // 缩放图片适应标签大小

    m_dynamicText = new QTextEdit(this);
    m_dynamicText->setGeometry(700, 155, 400, 180);
    m_dynamicText->setReadOnly(true);
    m_dynamicText->setStyleSheet("background-color:transparent; color:rgb(108,83,241) ; font-size: 24px; padding: 5px;");

    // 生成随机内容
    initDynamicContent();
}

PokeWindow::~PokeWindow() {}

void PokeWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg("poke_bg.png");   // 背景图
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), QColor(30, 30, 40));
        qDebug() << "未找到背景图 poke_bg.png";
    }
    QWidget::paintEvent(event);
}

void PokeWindow::onBackClicked()    // 返回主界面
{
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

void PokeWindow::initDynamicContent()   // 随机人物和文本
{
    // 生成 1~3 之间的随机数
    int r = QRandomGenerator::global()->bounded(1, 4);
    QString imagePath;
    QString textContent;

    // 预留功能：随机玩家成绩并且输出评价文本

    switch (r) {
    case 1:
        imagePath = "poke1.png";
        textContent = "戳到本蛋了喵呜！\n你这个蛋ꐦ";
        break;
    case 2:
        imagePath = "poke2.png";
        textContent = "这么喜欢戳我？欺负我没法戳你是吧，已将你的账号列入黑名单，"
                      "请Vxxy4216 50吃疯狂星期四解封，要不然别想玩Meowthm了喵～";
        break;
    case 3:
        imagePath = "poke3.png";
        textContent = "滚木的消息被设为了精华消息";
        break;
    default:
        imagePath = "poke1.png";
        textContent = "出错了喵";
        break;
    }

    QPixmap pix(imagePath); // 随机人物图片加载
    if (!pix.isNull()) {
        m_dynamicImage->setPixmap(pix);
    } else {
        // 如果图片加载失败，显示文字或默认颜色
        m_dynamicImage->setStyleSheet("background-color: gray;");
        qDebug() << "未找到图片" << imagePath;
    }

    m_dynamicText->setPlainText(textContent);
}

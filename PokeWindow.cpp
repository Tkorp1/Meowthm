// PokeWindow.cpp
#include "PokeWindow.h"
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QDebug>
#include <QMouseEvent>

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
    m_fixedText2->setPlainText("");
    m_fixedText2->setReadOnly(true);
    m_fixedText2->setStyleSheet("background-color: transparent; font-size: 14px;");

    // 返回按钮
    m_backBtn = new QPushButton("", this);
    m_backBtn->setGeometry(50, 730, 75, 50);
    m_backBtn->setStyleSheet("background-color:transparent; color: white; font-size: 16px; border-radius: 8px;");
    connect(m_backBtn, &QPushButton::clicked, this, &PokeWindow::onBackClicked);

    // 动态图片和第一个动态文本框（人物语录）
    m_dynamicImage = new QLabel(this);
    m_dynamicImage->setGeometry(492, 116, 200, 250);
    m_dynamicImage->setScaledContents(true);

    m_dynamicText = new QTextEdit(this);
    m_dynamicText->setGeometry(700, 155, 400, 180);
    m_dynamicText->setReadOnly(true);
    m_dynamicText->setStyleSheet("background-color:transparent; color:rgb(108,83,241) ; font-size: 24px; padding: 5px;");

    // 新增的随机文本框
    m_extraText = new QTextEdit(this);
    m_extraText->setGeometry(700, 400, 400, 250);
    m_extraText->setReadOnly(true);
    m_extraText->setStyleSheet("background-color:transparent; color:black; font-size: 18px;"); // 黑色字体，多行
    m_extraText->setPlainText(""); // 初始为空

    // 生成随机内容
    initDynamicContent();
}

PokeWindow::~PokeWindow() {}

void PokeWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg(":/image/poke_bg.png");   // 背景图
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), QColor(30, 30, 40));
        qDebug() << "未找到背景图 poke_bg.png";
    }
    QWidget::paintEvent(event);
}

void PokeWindow::onBackClicked()
{
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

void PokeWindow::initDynamicContent()
{
    int r = QRandomGenerator::global()->bounded(1, 4); // 1,2,3
    QString imagePath;
    QString textContent;      // 第一个文本框（人物语录）
    QString extraContent;     // 第二个文本框（额外随机文本）

    switch (r) {
    case 1:
        imagePath = ":/image/poke1.png";
        textContent = "戳到本蛋了喵呜！\n你这个蛋ꐦ";
        extraContent = "智慧小咸鱼，三个蛋中的炸蛋，Meowthm项目的主力，负责了工程量巨大的许多页面的美化和修bug"
                       "因此这个游戏也选用了这个蛋喜欢的风格\n\\o/咸鱼大人\\o/是蛋中的太阳，领导我们从一个胜利走向另一个胜利！"
                       "（注：此段文字疑似某🥚在被肘怕了的情况下写出）"
                        "\n“本人严肃声明：咸鱼不是区！！！”";
        break;
    case 2:
        imagePath = ":/image/poke2.png";
        textContent = "这么喜欢戳我？欺负我没法戳你是吧，已将你的账号列入黑名单，"
                      "请Vxxy4216 50吃疯狂星期四解封，要不然别想玩Meowthm了喵～";
        extraContent = "xueqiu，三个蛋中唯一真正蛋，把剩下两个人类带坏成为蛋了。因为喜欢跳舞的线，选用了雪球这个名字，实际上"
                       "和雪球完全相反，像一个火方，又或者更贴切的，蛋。提供了这个游戏大部分的搞笑点子。"
                       "\n“没错，这个令人忍俊不禁的戳页面，就是本蛋负责的！”";
        break;
    case 3:
        imagePath = ":/image/poke3.png";
        textContent = "滚木的消息被设为了精华消息";
        extraContent = "Tkorp，4kex2，phigros16.9，arcaea13.00，舞萌伪装成15834的16601，在Dynamix范式起源等领域也很强，是"
                       "毋庸置疑的音游大神（实际上这游戏的谱就他一个人能玩）平日里也是修数双的大学神，看起来和蛋一点都不沾边"
                        "\n可是在某些时候，也是会独自想□□□。！的滚木";
        break;
    default:
        imagePath = ":/image/poke1.png";
        textContent = "出错了喵";
        extraContent = "【错误】\n随机数异常。";
        break;
    }

    QPixmap pix(imagePath);
    if (!pix.isNull()) {
        m_dynamicImage->setPixmap(pix);
    } else {
        m_dynamicImage->setStyleSheet("background-color: gray;");
        qDebug() << "未找到图片" << imagePath;
    }

    m_dynamicText->setPlainText(textContent);
    m_extraText->setPlainText(extraContent);
}

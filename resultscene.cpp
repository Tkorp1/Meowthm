#include "resultscene.h"
#include <QFontDatabase>
#include <QFont>
#include <QPainter>

// 如果之后要返回主菜单
#include "mainwindow.h"

ResultScene::ResultScene(const GameState& _state,
                         QWidget *parent)
    : QWidget(parent),
    state(_state)
{
    int fontId =
        QFontDatabase::addApplicationFont(
            ":/static/Orbitron-Bold.ttf"
            );
    QString fontFamily =

        QFontDatabase::applicationFontFamilies(fontId)

            .at(0);
    // =========================
    // 1.窗口
    // =========================
    resize(1600,900);
    showFullScreen();

    setStyleSheet(
        "background-color:#2b2b2b;"
        );

    // =========================
    // 2.标题
    // =========================

    songNameLabel = new QLabel(this);

    songNameLabel->setText(
        QString(state.getCurrentSong())
        );
    songNameLabel->setGeometry(
        80,
        60,
        500,
        80
        );

    songNameLabel->setStyleSheet(
        "color:white;"
        "font-size:42px;"
        "font-weight:bold;"
        );

    songNameLabel->setAlignment(Qt::AlignLeft);

    // =========================
    // 3.Score
    // =========================

    scoreLabel = new QLabel(this);

    scoreLabel->setText(
        QString("%1")
            .arg(state.getCurrentScore())
        );
    scoreLabel->setGeometry(
        820,
        120,
        500,
        150
        );
    QFont scoreFont(
        fontFamily,
        72,
        QFont::Bold
        );

    scoreLabel->setFont(scoreFont);
    scoreLabel->setAlignment(Qt::AlignCenter);

    // =========================
    // 4.Acc
    // =========================

    accLabel = new QLabel(this);

    accLabel->setText(
        QString("Acc    %1%")
            .arg(
                state.getCurrentAcc()*100,
                0,
                'f',
                2
                )
        );

    accLabel->setGeometry(
        980,
        350,
        300,
        60
        );

    QFont accFont(

        fontFamily,

        28

        );

    accLabel->setFont(accFont);
    // =========================
    // 5.Combo
    // =========================

    comboLabel = new QLabel(this);

    comboLabel->setText(
        QString("Max Combo    %1")
            .arg(state.getMaxCombo())
        );

    comboLabel->setGeometry(
        850,
        320,
        500,
        60
        );
    comboLabel->setFont(accFont);
    // =========================
    // 6.Perfect
    // =========================

    perfectLabel = new QLabel(this);

    perfectLabel->setText(
        QString("Perfect    %1")
            .arg(state.getCurrentPerfect())
        );
    perfectLabel->setGeometry(
        850,
        430,
        500,
        50
        );

    perfectLabel->setStyleSheet(
        "color:white;"
        "font-size:20px;"
        );

    // =========================
    // 7.Good
    // =========================

    goodLabel = new QLabel(this);

    goodLabel->setText(
        QString("Good       %1")
            .arg(state.getCurrentGood())
        );
    goodLabel->setGeometry(
        850,
        500,
        500,
        50
        );

    goodLabel->setStyleSheet(
        "color:white;"
        "font-size:20px;"
        );

    // =========================
    // 8.Miss
    // =========================

    missLabel = new QLabel(this);

    missLabel->setText(
        QString("Miss         %1")
            .arg(state.getCurrentMiss())
        );
    missLabel->setGeometry(
        850,
        570,
        500,
        50
        );

    missLabel->setStyleSheet(
        "color:white;"
        "font-size:20px;"
        );

    // =========================
    // 9.返回按钮
    // =========================

    returnButton = new QPushButton(
        "Return",
        this
        );
    returnButton->setGeometry(
        850,
        700,
        250,
        60
        );

    connect(
        returnButton,
        &QPushButton::clicked,
        this,
        &ResultScene::onReturnMainMenu
        );

    songNameLabel->show();
    scoreLabel->show();
    accLabel->show();
    comboLabel->show();
    perfectLabel->show();
    goodLabel->show();
    missLabel->show();
    returnButton->show();
}

ResultScene::~ResultScene()
{

}

void ResultScene::onReturnMainMenu()
{
    MainWindow* menu =
        new MainWindow();

    menu->show();

    close();
}
void ResultScene::paintEvent(QPaintEvent *event)

{
    QWidget::paintEvent(event);

    QPainter painter(this);

    QPixmap bg(":/image/images/resultScene.jpg");

    painter.drawPixmap(this->rect(), bg);

    painter.setRenderHint(QPainter::Antialiasing);

    QPolygon panel;

    panel << QPoint(780,40)
          << QPoint(1450,40)
          << QPoint(1350,860)
          << QPoint(680,860);

    painter.setPen(Qt::NoPen);

    painter.setBrush(
        QColor(10,10,10,150)
        );

    painter.drawPolygon(panel);
}
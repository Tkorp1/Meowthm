#include "resultscene.h"

#include <QFont>
#include <QPainter>

// 如果之后要返回主菜单
#include "mainwindow.h"

ResultScene::ResultScene(const GameState& _state,
                         QWidget *parent)
    : QWidget(parent),
    state(_state)
{
    // =========================
    // 1.窗口
    // =========================

    setFixedSize(800,600);

    setStyleSheet(
        "background-color:#2b2b2b;"
        );

    // =========================
    // 2.标题
    // =========================

    songNameLabel = new QLabel(this);

    songNameLabel->setText(
        QString("Result")
        );

    songNameLabel->setGeometry(
        250,
        30,
        300,
        60
        );

    songNameLabel->setStyleSheet(
        "color:white;"
        "font-size:36px;"
        "font-weight:bold;"
        );

    // =========================
    // 3.Score
    // =========================

    scoreLabel = new QLabel(this);

    scoreLabel->setText(
        QString("Score : %1")
            .arg(state.getCurrentScore())
        );

    scoreLabel->setGeometry(
        250,
        120,
        300,
        50
        );

    scoreLabel->setStyleSheet(
        "color:#FFD700;"
        "font-size:28px;"
        );

    // =========================
    // 4.Acc
    // =========================

    accLabel = new QLabel(this);

    accLabel->setText(
        QString("Acc : %1%")
            .arg(
                state.getCurrentAcc()*100,
                0,
                'f',
                2
                )
        );

    accLabel->setGeometry(
        250,
        180,
        300,
        50
        );

    accLabel->setStyleSheet(
        "color:#98FB98;"
        "font-size:24px;"
        );

    // =========================
    // 5.Combo
    // =========================

    comboLabel = new QLabel(this);

    comboLabel->setText(
        QString("Max Combo : %1")
            .arg(state.getMaxCombo())
        );

    comboLabel->setGeometry(
        250,
        240,
        300,
        50
        );

    comboLabel->setStyleSheet(
        "color:#00BFFF;"
        "font-size:24px;"
        );

    // =========================
    // 6.Perfect
    // =========================

    perfectLabel = new QLabel(this);

    perfectLabel->setText(
        QString("Perfect : %1")
            .arg(state.getCurrentPerfect())
        );

    perfectLabel->setGeometry(
        250,
        320,
        250,
        40
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
        QString("Good : %1")
            .arg(state.getCurrentGood())
        );

    goodLabel->setGeometry(
        250,
        370,
        250,
        40
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
        QString("Miss : %1")
            .arg(state.getCurrentMiss())
        );

    missLabel->setGeometry(
        250,
        420,
        250,
        40
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
        300,
        500,
        200,
        50
        );

    connect(
        returnButton,
        &QPushButton::clicked,
        this,
        &ResultScene::onReturnMainMenu
        );
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

void ResultScene::paintEvent(QPaintEvent *event){

}
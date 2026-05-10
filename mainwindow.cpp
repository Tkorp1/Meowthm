#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene=new QGraphicsScene(this);
    view=new QGraphicsView(scene, this);
    setCentralWidget(view);
    resize(800,600);
    int laneWidth=100;
    for(int i=0;i<4;i++){
        scene->addRect(i*laneWidth, 0, laneWidth, 600);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


// PokeWindow.h
#ifndef POKEWINDOW_H
#define POKEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

class PokeWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PokeWindow(QWidget *parent = nullptr);
    ~PokeWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onBackClicked();

private:
    void initDynamicContent();   // 根据随机数初始化动态内容

    QLabel      *m_dynamicImage;   // 动态图片
    QTextEdit   *m_dynamicText;    // 动态文本框
    QPushButton *m_backBtn;        // 返回按钮

    // 两个固定的多行文本框
    QTextEdit *m_fixedText1;
    QTextEdit *m_fixedText2;

    //void mousePressEvent(QMouseEvent *event) override;  // 调试用鼠标位置显示
};

#endif // POKEWINDOW_H

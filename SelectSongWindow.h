#ifndef SELECTSONGWINDOW_H
#define SELECTSONGWINDOW_H

#include <QWidget>

#include <QList>

#include <QPushButton>
#include <QLabel>   // 新增：用于显示图片


class SelectSongWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SelectSongWindow(QWidget *parent = nullptr);
    ~SelectSongWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:

    void onSettings();      // 设置界面
    void onPoke();          // 戳界面
    void onProfile();       // 个人档案
    void onAchievements();  // 成就界面
    void onBackToMain();    // 返回主界面

    // 点击歌曲卡片槽函数
    void onSongCardClicked();

    void onRecord();


private:
    QList<QPushButton*> m_cardButtons;

    // 新增：四张图片控件
    QLabel *m_image1;
    QLabel *m_image2;
    QLabel *m_image3;
    QLabel *m_image4;

};

#endif // SELECTSONGWINDOW_H

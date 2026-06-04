#ifndef SELECTSONGWINDOW_H
#define SELECTSONGWINDOW_H

#include <QWidget>
#include <QList>
#include <QPushButton>

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

private:
    // 【存放左侧歌曲按钮的列表
    QList<QPushButton*> m_cardButtons;
};

#endif // SELECTSONGWINDOW_H

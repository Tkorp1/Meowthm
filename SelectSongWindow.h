#ifndef SELECTSONGWINDOW_H
#define SELECTSONGWINDOW_H

#include <QWidget>

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

private:

};

#endif // SELECTSONGWINDOW_H

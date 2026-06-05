#ifndef ACHIEVEMENTSWINDOW_H
#define ACHIEVEMENTSWINDOW_H

#include <QWidget>

class AchievementsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit AchievementsWindow(QWidget *parent = nullptr);
    ~AchievementsWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ACHIEVEMENTSWINDOW_H

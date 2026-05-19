#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QWidget>

class ProfileWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileWindow(QWidget *parent = nullptr);
    ~ProfileWindow();
};

#endif // PROFILEWINDOW_H

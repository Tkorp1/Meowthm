#ifndef SELECTSONGWINDOW_H
#define SELECTSONGWINDOW_H

#include <QWidget>

class SelectSongWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SelectSongWindow(QWidget *parent = nullptr);
    ~SelectSongWindow();
};

#endif // SELECTSONGWINDOW_H

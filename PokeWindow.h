#ifndef POKEWINDOW_H
#define POKEWINDOW_H

#include <QWidget>

class PokeWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PokeWindow(QWidget *parent = nullptr);
    ~PokeWindow();
};

#endif // POKEWINDOW_H

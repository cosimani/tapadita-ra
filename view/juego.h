#ifndef JUEGO_H
#define JUEGO_H

#include <QWidget>

namespace Ui {
class Juego;
}

class Juego : public QWidget
{
    Q_OBJECT

public:
    explicit Juego(QWidget *parent = 0);
    ~Juego();

    void initJuego();

private:
    Ui::Juego *ui;
};

#endif // JUEGO_H

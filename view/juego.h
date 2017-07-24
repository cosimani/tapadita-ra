#ifndef JUEGO_H
#define JUEGO_H

#include <QWidget>
#include "view/puntaje.h"

namespace Ui {
class Juego;
}

class Juego : public QWidget
{
    Q_OBJECT

public:
    explicit Juego(QWidget *parent = 0);
    ~Juego();

    Ui::Juego *ui;
    QVector<Puntaje *> puntajes;

    void initJuego();
    void cargarCamaras();


    QVector<Puntaje *> getPuntajes() const;
    void setPuntajes(const QVector<Puntaje *> &value);

private slots:
    void slot_cbCamarasChanged(int nuevoIndex);
};

#endif // JUEGO_H

#ifndef JUEGO_H
#define JUEGO_H

#define MINMARKERSIZE 50
#define MAXMARKERSIZE 200

#include <QWidget>
#include <QString>
#include "view/puntaje.h"
#include "view/settings.h"
#include <QDebug>

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

    Settings * settings;

    void initJuego();
    void cargarCamaras();


    QVector<Puntaje *> getPuntajes() const;
    void setPuntajes(const QVector<Puntaje *> &value);

private slots:
    void slot_cbCamarasChanged(int nuevoIndex);
    void slot_Settings();
    void slot_MarkerSize(int size);
    void slot_newProfilePicture(QString newPath, int player);
};

#endif // JUEGO_H

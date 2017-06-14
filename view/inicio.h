#ifndef INICIO_H
#define INICIO_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include "database.hpp"
#include "factory.h"
#include "controller/jugador.h"

namespace Ui {
class Inicio;
}

class Inicio : public QWidget
{
    Q_OBJECT

    QVector<Jugador *> jugadoresActuales;
    QMap<QString,QString> currentPlayers;
    QMap<QString,QString> currentFJ;

public:
    explicit Inicio(QWidget *parent = 0);
    ~Inicio();

    /**
     * @brief se llama al crearse el objeto, para setear valores a las imagenes, label, y buton por defecto
     */
    void initDefaultValues();

    /**
     * @brief insertPlayersInDB inserto cada jugador en la bd
     * @return true si inserto TODOS
     */
    bool insertPlayersInDB();

    /**
     * @brief insertFichasJugadoresInDB inserto la relacion de
     * cada ficha(tarjeta) que de cada jugador en la base de datos
     * @return true si inserto TODOS
     */
    bool insertVinculosInDB();

    /**
     * @brief registrarJugadores guarda los el valor del campo nombre y la ruta de la foto de perfil
     * de cada jugador. Los datos de cada jugador se toman de cada RegisterPlayer en la vista
     * @return true si TODOS los jugadores fueron insertados, si algun fallo false.
     */
    bool registrarJugadores();
    QMap<QString, QString> getCurrentPlayers() const;
    void setCurrentPlayers(const QMap<QString, QString> &value);

    QMap<QString, QString> getCurrentFJ() const;
    void setCurrentFJ(const QMap<QString, QString> &value);

    QVector<Jugador *> getJugadoresActuales() const;
    void setJugadoresActuales(const QVector<Jugador *> &value);

private:
    Ui::Inicio *ui;

private slots:
    void slot_start(bool push);

signals:
    void sig_start(QMap<QString, QString> fichas_jugadores);
};

#endif // INICIO_H

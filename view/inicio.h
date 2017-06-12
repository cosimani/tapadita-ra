#ifndef INICIO_H
#define INICIO_H

#include <QWidget>
#include <QMap>
#include "database.hpp"

namespace Ui {
class Inicio;
}

class Inicio : public QWidget
{
    Q_OBJECT

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
     * @return true si fue exitoso
     */
    bool insertPlayersInDB(QMap<QString,QString> players);

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

private:
    Ui::Inicio *ui;

private slots:
    void slot_start(bool push);

signals:
    void sig_start(QMap<QString, QString> fichas_jugadores);
};

#endif // INICIO_H

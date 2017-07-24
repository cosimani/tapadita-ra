#ifndef JUGADOR_H
#define JUGADOR_H

#include <QString>
#include <QVector>
#include <QColor>
#include <QPoint>
#include <QMap>
#include "model/qrcode.h"

#include "view/puntaje.h"

/* Jr del futuro:
 *
 * 2- podria hacer un vector con los id que se estan mostrando
 */

class Jugador
{
    int nro_jugador;
    QString nombre;
    QString foto_perfil;
    QString foto_triangulacion;
    QString foto_muerte;

    // fichas de cada jugador
    QVector<QRCode *> * fichas;

    // vectores de markers asociados
    QVector<int> * vecids;

    // color de marker ganador
    QColor win_color;

    // color de la triangulacion del equipo
    QColor teamColor;

    int puntos;

    Puntaje * puntaje;

    static QVector<Jugador*> * jugadoresActuales;

public:

    Jugador();

    static QVector<Jugador *> * getJugadoresActuales(){
        if(jugadoresActuales == NULL) {
            jugadoresActuales = new QVector<Jugador*>();
            return jugadoresActuales;
        }
        return jugadoresActuales;
    } 

    void updatePuntaje();

    // Getters And Setters
    int getNro_jugador() const;
    void setNro_jugador(int value);
    QString getNombre() const;
    void setNombre(const QString &value);
    QString getFoto_perfil() const;
    void setFoto_perfil(const QString &value);
    QVector<int> *getVecids() const;
    void setVecids(QVector<int> *value);
    QColor getWin_color() const;
    void setWin_color(const QColor &value);
    QColor getTeamColor() const;
    void setTeamColor(const QColor &value);
    QVector<QRCode *> *getFichas() const;
    void setFichas(QVector<QRCode *> *value);
    int getPuntos() const;
    void setPuntos(int value);
    Puntaje *getPuntaje() const;
    void setPuntaje(Puntaje *value);
    QString getFoto_triangulacion() const;
    void setFoto_triangulacion(const QString &value);
    QString getFoto_muerte() const;
    void setFoto_muerte(const QString &value);
};

#endif // JUGADOR_H

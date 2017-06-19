#ifndef JUGADOR_H
#define JUGADOR_H

#include <QString>
#include <QVector>
#include <QColor>

class Jugador
{
    int nro_jugador;
    QString nombre;
    QString foto_perfil;
    // vectores de markers asociados
    QVector<int> * vecids;

    // color de marker ganador
    QColor win_color;

    // color de la triangulacion del equipo
    QColor teamColor;

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
};

#endif // JUGADOR_H

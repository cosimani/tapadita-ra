#ifndef JUGADOR_H
#define JUGADOR_H

#include <QString>
#include <QVector>

class Jugador
{
    int nro_jugador;
    QString nombre;
    QString foto_perfil;

    static QVector<Jugador*> * jugadoresActuales;

    QVector<int> * vecids;

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
};

#endif // JUGADOR_H

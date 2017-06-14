#include "jugador.h"

QVector<Jugador*>* Jugador::jugadoresActuales = NULL;

QVector<int> *Jugador::getVecids() const
{
    return vecids;
}

void Jugador::setVecids(QVector<int> *value)
{
    vecids = value;
}

Jugador::Jugador(): nro_jugador(-1),
    nombre(""),
    foto_perfil(""),
    vecids(new QVector<int>())
{
    
}

//---------------------- Getters and setters-------------------------
int Jugador::getNro_jugador() const
{
    return nro_jugador;
}

void Jugador::setNro_jugador(int value)
{
    nro_jugador = value;
}

QString Jugador::getNombre() const
{
    return nombre;
}

void Jugador::setNombre(const QString &value)
{
    nombre = value;
}

QString Jugador::getFoto_perfil() const
{
    return foto_perfil;
}

void Jugador::setFoto_perfil(const QString &value)
{
    foto_perfil = value;
}


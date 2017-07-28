#include "jugador.h"

QVector<Jugador*>* Jugador::jugadoresActuales = NULL;

Jugador::Jugador(): nro_jugador(-1),
    nombre(""),
    foto_perfil(""),
    foto_muerte(""),
    foto_triangulacion(""),
    vecids(new QVector<int>()),
    fichas(new QVector<QRCode *>()),
    win_color(0,0,0),
    teamColor(255,255,255),
    puntos(0)
{

}

/**
 * @brief Jugador::updatePuntaje actualizo atributo puntajes
 * y tambien la interfaz con el nuevo valor.
 * @param puntos nuevo puntaje
 */
void Jugador::updatePuntaje()
{
    int pts = 0;
    for(int i = 0; i < this->getFichas()->size(); i++){
        QRCode * qrc = this->getFichas()->at(i);
//        qDebug() << "qrc points:" << qrc->getPuntaje();
//        qDebug() << qrc->getCordY();

        pts += qrc->getPuntaje();
    }

    this->setPuntos(pts);
    this->getPuntaje()->updatePuntos(pts);
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

QVector<int> *Jugador::getVecids() const
{
    return vecids;
}

void Jugador::setVecids(QVector<int> *value)
{
    vecids = value;
}

QColor Jugador::getWin_color() const
{
    return win_color;
}

void Jugador::setWin_color(const QColor &value)
{
    win_color = value;
}

QColor Jugador::getTeamColor() const
{
    return teamColor;
}

void Jugador::setTeamColor(const QColor &value)
{
    teamColor = value;
}

QVector<QRCode *> *Jugador::getFichas() const
{
    return fichas;
}

void Jugador::setFichas(QVector<QRCode *> *value)
{
    fichas = value;
}

int Jugador::getPuntos() const
{
    return puntos;
}

void Jugador::setPuntos(int value)
{
    puntos = value;
}

Puntaje *Jugador::getPuntaje() const
{
    return puntaje;
}

void Jugador::setPuntaje(Puntaje *value)
{
    puntaje = value;
}

QString Jugador::getFoto_triangulacion() const
{
    return foto_triangulacion;
}

void Jugador::setFoto_triangulacion(const QString &value)
{
    foto_triangulacion = value;
}

QString Jugador::getFoto_muerte() const
{
    return foto_muerte;
}

void Jugador::setFoto_muerte(const QString &value)
{
    foto_muerte = value;
}

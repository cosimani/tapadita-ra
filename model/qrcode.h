#ifndef QRCODE_H
#define QRCODE_H

#include <QVector>
#include <QPoint>
#include <QDebug>
#include "aruco/marker.h"

using namespace aruco;

class QRCode
{
    Marker mkr;

    // posiciones x del marker
    float cordX;

    // posiciones y del marker
    float cordY;

    // player activo
    bool visible;

    // distancia a la linea
    float distanceToTarget;

    // puntaje del qrcode segun la distancia al target.
    int puntaje;

public:

    enum ESTADO {valid, dead, triangulation};

    ESTADO estado;

    QRCode();

    static QRCode *createQRC(Marker mkr, float cordX = -1, float cordY = -1 , bool activo = false, ESTADO estado = ESTADO::valid);
    static QRCode *createQRC();

    void updatePuntaje(int puntaje);

    bool getVisible() const;
    void setVisible(bool value);
    float getCordX() const;
    void setCordX(float value);
    float getCordY() const;
    void setCordY(float value);
    Marker getMkr() const;
    void setMkr(const Marker &value);
    ESTADO getEstado() const;
    void setEstado(const ESTADO &value);
    float getDistanceToTarget() const;
    void setDistanceToTarget(float value);
    int getPuntaje() const;
    void setPuntaje(int value);
};

#endif // QRCODE_H

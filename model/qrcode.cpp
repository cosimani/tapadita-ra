#include "qrcode.h"

QRCode::QRCode():
    cordX(0),
    cordY(0),
    visible(false),
    distanceToTarget(-1)
{
    
}

QRCode *QRCode::createQRC(Marker mkr, float cordX, float cordY, bool visible, ESTADO estado)
{
    QRCode * qrc = new QRCode();
    qrc->setMkr(mkr);
    qrc->setCordX(cordX);
    qrc->setCordY(cordY);
    qrc->setVisible(visible);
    qrc->setEstado(estado);

    return qrc;
}

QRCode *QRCode::createQRC()
{
    Marker m;
    return createQRC(m, -1, -1, false);
}


//------------------------------Getters And Setters---------------------------------

bool QRCode::getVisible() const
{
    return visible;
}

void QRCode::setVisible(bool value)
{
    visible = value;
}

float QRCode::getCordX() const
{
    return cordX;
}

void QRCode::setCordX(float value)
{
    cordX = value;
}

float QRCode::getCordY() const
{
    return cordY;
}

void QRCode::setCordY(float value)
{
    cordY = value;
}

Marker QRCode::getMkr() const
{
    return mkr;
}

void QRCode::setMkr(const Marker &value)
{
    mkr = value;
}

QRCode::ESTADO QRCode::getEstado() const
{
    return estado;
}

void QRCode::setEstado(const ESTADO &value)
{
    estado = value;
}

float QRCode::getDistanceToTarget() const
{
    return distanceToTarget;
}

void QRCode::setDistanceToTarget(float value)
{
    distanceToTarget = value;
}

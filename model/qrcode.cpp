#include "qrcode.h"

QRCode::QRCode():
    cordX(0),
    cordY(0),
    visible(false)
{
    
}

QRCode *QRCode::createQRC(Marker mkr, float cordX, float cordY, bool visible)
{
    QRCode * qrc = new QRCode();
    qrc->setMkr(mkr);
    qrc->setCordX(cordX);
    qrc->setCordY(cordY);
    qrc->setVisible(visible);

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

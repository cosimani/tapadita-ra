#include "mibutton.h"
#include <QDebug>
#include <QtConcurrent/qtconcurrentexception.h>

MiButton::MiButton(QWidget *parent): QPushButton(parent)
{
}


void MiButton::mousePressEvent(QMouseEvent *)
{
    profilePicturePath = QFileDialog::getOpenFileName(this, "Seleccionar foto de perfil", "../", "Multimedia (*.png *.jpg)");
    if(profilePicturePath.isEmpty() || profilePicturePath.isNull()){
        qDebug() << "mousePressEvent: No se selecciono imagen";
    }else{
        emit sig_imagePath(profilePicturePath);
    }
}

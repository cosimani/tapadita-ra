#include "registerplayer.h"
#include "ui_registerplayer.h"

#include<QDebug>

/**
 *  TERMINAR ESTO: FALTAN QUE REGISTER PLAYER TOME ESTO Y SETEAR A LAS VISTAS LOS CAMBIOS
 */

RegisterPlayer::RegisterPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPlayer)
{
    ui->setupUi(this);

    ui->mctName->setMaximumHeight(this->height()/8);
    ui->mctName->setTextToLabel("nombre");
}

RegisterPlayer::~RegisterPlayer()
{
    delete ui;
}

// ---------------- getters and setters --------------------------
QString RegisterPlayer::getPathFotoPerfil()
{
    pathFotoPerfil = ui->pp->getPathFoto_perfil();
    return pathFotoPerfil;
}

void RegisterPlayer::setPathFotoPerfil(const QString &value)
{
    pathFotoPerfil = value;
}

QString RegisterPlayer::getUserName()
{
    userName = ui->mctName->getUserName();
    return userName;
}

void RegisterPlayer::setUserName(const QString &value)
{
    userName = value;
    ui->mctName->setTextToFild(userName);
}

#include "profilepicture.h"
#include "ui_profilepicture.h"
#include "common.h"


ProfilePicture::ProfilePicture(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfilePicture),
    pathFotoPerfil(APPLICATION_PATH "images/empty_profile2.jpg")
{
    ui->setupUi(this);

    initDefaultValues();
    connect(ui->mbBrowse, SIGNAL(sig_imagePath(QString)), this, SLOT(slot_loadImage(QString)));
}

ProfilePicture::~ProfilePicture()
{
    delete ui;
}


void ProfilePicture::initDefaultValues()
{
    ui->mbBrowse->setText("browse");
    ui->imProfile->setImage( pathFotoPerfil, false);
    ui->lCabecera->setMaximumHeight(this->height()/8);
    ui->lCabecera->setText("seleccionar foto de perfil");
}

void ProfilePicture::slot_loadImage(QString path)
{
    setPathFoto_perfil(path);
    ui->imProfile->setImage(path, false);
}

//---------------------------- getters and setters ------------------------------------

QString ProfilePicture::getPathFoto_perfil() const
{
    return pathFotoPerfil;
}

void ProfilePicture::setPathFoto_perfil(const QString &value)
{
    pathFotoPerfil = value;
}

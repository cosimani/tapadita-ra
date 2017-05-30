#include "profilepicture.h"
#include "ui_profilepicture.h"
#include "common.h"

ProfilePicture::ProfilePicture(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfilePicture)
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
    ui->imProfile->setImage( APPLICATION_PATH "images/empty_profile2.jpg", false);
    ui->lCabecera->setMaximumHeight(this->height()/8);
    ui->lCabecera->setText("seleccionar foto de perfil");
}

void ProfilePicture::slot_loadImage(QString path)
{
    ui->imProfile->setImage(path, false);
}

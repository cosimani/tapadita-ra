#include "registerplayer.h"
#include "ui_registerplayer.h"

#include<QDebug>

RegisterPlayer::RegisterPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPlayer)
{
    ui->setupUi(this);

//    connect(ui->pbStart, SIGNAL(clicked(bool)), this, SLOT(slot_start(bool)));

//    ui->pp->setMaximumHeight(this->height());
//    ui->pp->setMinimumWidth(this->width());

    ui->mctName->setMaximumHeight(this->height()/8);
    ui->mctName->setTextToLabel("nombre");
}

RegisterPlayer::~RegisterPlayer()
{
    delete ui;
}

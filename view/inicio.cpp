#include "inicio.h"
#include "ui_inicio.h"

#include <QDebug>

Inicio::Inicio(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Inicio)
{
    ui->setupUi(this);

    this->initDefaultValues();
    connect(ui->pbstart, SIGNAL(clicked(bool)), this, SLOT(slot_start(bool)));
}

Inicio::~Inicio()
{
    delete ui;
}

void Inicio::initDefaultValues()
{
    ui->ltitle->setMaximumHeight(this->height()/5);
    ui->rp1->setMaximumHeight(this->height()*1.5);
    ui->rp2->setMaximumHeight(this->height()*1.5);
}


void Inicio::slot_start(bool push)
{
    qDebug() << "slot_start, se pulso button";
    emit sig_start();
}

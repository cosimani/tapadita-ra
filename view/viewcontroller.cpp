#include "viewcontroller.h"
#include "ui_viewcontroller.h"

#include<QDebug>

ViewController::ViewController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewController)
{
    ui->setupUi(this);

    this->initDefaultValues();

    connect(ui->inicio, SIGNAL(sig_start()), this, SLOT(slot_showPrincipal()));
}

ViewController::~ViewController()
{
    delete ui;
}


void ViewController::initDefaultValues()
{
    ui->inicio->setVisible(true);
    ui->principal->setVisible(false);
}

void ViewController::slot_showPrincipal()
{
    qDebug() << "slot_showPrincipal: se oculta register y muestra principal";
    ui->inicio->setVisible(false);
    ui->principal->setVisible(true);
    // inicializo el timer de scene aca, porque sino esta visible y se intenta actualizar la esena rompe
    ui->principal->startTimer();
}

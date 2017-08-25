#include "puntaje.h"
#include "ui_puntaje.h"
#include <QString>

Puntaje::Puntaje(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Puntaje)
{
    ui->setupUi(this);
}

Puntaje::~Puntaje()
{
    delete ui;
}

void Puntaje::updatePuntos(int puntos)
{
    ui->lPuntos->setText( QString::number(puntos) );
}

void Puntaje::setPuntajeName(QString name)
{
    ui->lName->setText(name);
}

QString Puntaje::getPlayerName(){
    return ui->lName->text();
}

#include "viewcontroller.h"
#include "ui_viewcontroller.h"

#include<QDebug>


ViewController::ViewController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewController)
{
    ui->setupUi(this);

    this->initDefaultValues();
    this->initDataBase();
    Database::getInstance()->getNumberOfRows("vinculos");
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

// estaria bueno hacer un factory que inicie la base de datos
// nota: demora bastante en insertar en la base de datos al comienzo,
// habria que hacerlo luego de que se creen las vistas, para que no
// se interrumpa O sino hacerlo en otro hilo con QThread
void ViewController::initDataBase()
{
    if (!Database::getInstance()->checkBase() == -1){
        qDebug() << "ERROR: No se pudo conectar con la base de datos";
        this->close();
    }
    qDebug() << "por cargar markers";
    // si ya se cargaron los marcadores, no los vuelvo a cargar
    int filas = Database::getInstance()->getNumberOfRows("vinculos");
    qDebug() << "cantidad de filas:" << filas;
    if (! (filas == CANTIDAD_MARCADORES) ) {
    qDebug() << "cargar markers";
    // aca tengo que cargar los datos de los marcadores
        for(int i = 0; i < CANTIDAD_MARCADORES; i++){
            QStringList values;
            values.append(QString::number(i));
            values.append("null");
            values.append("null");
            Database::getInstance()->insert_into("vinculos", values);
            qDebug() << "cargar markers adentro" << i;
        }
    }
}



void ViewController::slot_showPrincipal()
{
    qDebug() << "slot_showPrincipal: se oculta register y muestra principal";
    ui->inicio->setVisible(false);
    ui->principal->setVisible(true);
    // inicializo el timer de scene aca, porque sino esta visible y se intenta actualizar la esena rompe
    ui->principal->startTimer();
}

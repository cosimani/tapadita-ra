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
    connect(ui->inicio, SIGNAL(sig_start(QMap<QString, QString>)), this, SLOT(slot_showPrincipal(QMap<QString, QString>)));
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

// creo que mas facil seria insertar los marcadores que uso y listo.
void ViewController::initDataBase()
{
    if (!Database::getInstance()->checkBase() == -1){
        qDebug() << "ERROR: No se pudo conectar con la base de datos";
        this->close();
    }

    // si ya se cargaron los marcadores, no los vuelvo a cargar
    int filas = Database::getInstance()->getNumberOfRows("vinculos");
    if (! (filas == CANTIDAD_MARCADORES) ) {
    qDebug() << "cargar markers";
    // aca tengo que cargar los datos de los marcadores
        for(int i = 0; i < 0; i++){
            QStringList values;
            values.append(QString::number(i));
            values.append("null");
            values.append("null");
            values.append("null");
            if(!Database::getInstance()->insert_into("vinculos", values)){
                qDebug() << "void ViewController::initDataBase(): No se pudo insertar en tabla vinculos";
            }
        }
    }
}



void ViewController::slot_showPrincipal(QMap<QString, QString> fichas_jugadores)
{
    qDebug() << "slot_showPrincipal: se oculta register y muestra principal";
    ui->inicio->setVisible(false);
    ui->principal->setVisible(true);
    ui->principal->setFichas_jugadores(fichas_jugadores);

    // inicializo el timer de scene aca, porque sino esta visible y se intenta actualizar la esena rompe
    ui->principal->startTimer();
}

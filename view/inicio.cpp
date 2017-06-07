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

    ui->rp1->setUserName("player 1");
    ui->rp2->setUserName("player 2");
}

bool Inicio::insertPlayersInDB(QMap<QString,QString> players)
{
    return Database::getInstance()->insert_into("jugadores", players);
}

// registra todos los jugadores tomando la lista de todos los widgets
// del tipo RegisterPlayer y por cada uno inserta un jugador
// deberia controlar que no existan en la base de datos (?)
bool Inicio::registrarJugadores(){
    qDebug() << ui->gridLayout->count();

    qDebug() << "entro a registrarJugador";
    for(int i = 0; i < ui->gridLayout->count();i++){
        RegisterPlayer * rp = qobject_cast<RegisterPlayer *>(ui->gridLayout->itemAt(i)->widget());
        if(rp == 0){
//            qDebug() << "widget ES null No inserto";
        }else{
//            qDebug() << "widget distinto de null";
            QMap <QString, QString> map;
            QString foto_perfil = rp->getPathFotoPerfil();
            foto_perfil.push_back("'");
            foto_perfil.push_front("'");

            QString nom_jugador = rp->getUserName();
            nom_jugador.push_back("'");
            nom_jugador.push_front("'");

            map["nom_jugador"] = nom_jugador;
            map["foto_perfil"] = foto_perfil;

            if(!this->insertPlayersInDB(map))
                return false;
        }
    }
    return true;
}

void Inicio::slot_start(bool push)
{
    if (!registrarJugadores()) {
        qDebug() << "ERROR: Inicio::slot_start no se pudieron registrar todos los jugadores";
        this->close();
    }

    qDebug() << "slot_start, se pulso button";
    emit sig_start();
}

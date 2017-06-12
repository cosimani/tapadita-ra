#include "inicio.h"
#include "ui_inicio.h"

#include <QDebug>

/* junior del futuro
 *
 * TODO:
 *
 * 1-ahora que se insertaron en las 3 tablas,
 * ver como pasarle los datos de las rutas de las fotos
 * a la clase Principal (o a Scene).
 *
 * 2-Despues dibujar la imagen en la tarjeta
 */

QMap<QString, QString> Inicio::getCurrentPlayers() const
{
    return currentPlayers;
}

void Inicio::setCurrentPlayers(const QMap<QString, QString> &value)
{
    currentPlayers = value;
}

QMap<QString, QString> Inicio::getCurrentFJ() const
{
    return currentFJ;
}

void Inicio::setCurrentFJ(const QMap<QString, QString> &value)
{
    currentFJ = value;
}

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
    QMap <QString, QString> map;
    for(int i = 0; i < ui->gridLayout->count();i++){
        RegisterPlayer * rp = qobject_cast<RegisterPlayer *>(ui->gridLayout->itemAt(i)->widget());
        if(rp == 0){
//            qDebug() << "widget ES null No inserto";
        }else{
//            qDebug() << "widget distinto de null";
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

    /* TODO:
     * controlar que el map tenga nombres validos
     */
    // seteo los jugadores actuales
    setCurrentPlayers(map);

    /* TODO:
     * una vez registrados los jugadores, debo asociarlos con un(os)
     * marcadores. para ello, tengo que insertar en la tabla
     * fichas_jugador el id del marcador y el nro de jugador
     *
     * Status: terminado
     *
     * VERIFICAR: el marker id va a corresponder con el de la tabla ?
    */

    qDebug() << "ejecuto getlastrow";
    int nplayer = Database::getInstance()->getLastRow("jugadores", "nro_jugador");

    QMap <QString, QString> auxMap;
    QString nro_jugador = QString::number(nplayer);
    nro_jugador.push_back("'");
    nro_jugador.push_front("'");
    QString marker_id = QString::number(51);
    marker_id.push_back("'");
    marker_id.push_front("'");
    auxMap["nro_jugador"] = nro_jugador;
    auxMap["marker_id"] = marker_id;
    nro_jugador.clear();
    marker_id.clear();
    Database::getInstance()->insert_into("fichas_jugador", auxMap);

    nro_jugador = QString::number(nplayer - 1);
    nro_jugador.push_back("'");
    nro_jugador.push_front("'");
    marker_id = QString::number(17);
    marker_id.push_back("'");
    marker_id.push_front("'");
    auxMap["nro_jugador"] = nro_jugador;
    auxMap["marker_id"] = marker_id;
    nro_jugador.clear();
    marker_id.clear();
    Database::getInstance()->insert_into("fichas_jugador", auxMap);
    setCurrentFJ(auxMap);

    return true;
}

void Inicio::slot_start(bool push)
{
    if (!registrarJugadores()) {
        qDebug() << "ERROR: Inicio::slot_start no se pudieron registrar todos los jugadores";
        this->close();
    }

    qDebug() << "slot_start, se pulso button";
    emit sig_start(this->getCurrentFJ());
}

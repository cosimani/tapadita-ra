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
 * Status: Terminado
 *
 * 2-Despues dibujar la imagen en la tarjeta
 *
 * Status: Terminado
 *
 * 3- Mejorar insercion en la base de datos, para la tabla fichas_jugadores
 * ver como hacerlo con un for o dentro del mismo donde se insertan los jugadores,
 *
 * podria agregar un atributo a Jugador, con la cantidad de fichas que tiene.
 *
 * Status: Terminado
 */

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



// registra todos los jugadores tomando la lista de todos los widgets
// del tipo RegisterPlayer y por cada uno inserta un jugador
// deberia controlar que no existan en la base de datos (?)
bool Inicio::registrarJugadores(){
    qDebug() << "entro a registrarJugador";

    bool ip = insertPlayersInDB();
    bool iv = insertVinculosInDB();

    return ip && iv;
}


/**
 * @brief Inicio::registrarJugadores busca todos los objetos del tipo RegisterPlayer
 * y por cada uno, toma el nombre y la foto de perfil que tiene el objeto, con eso creo
 * cada jugador, que se inserta en la base de datos.
 * Al mismo tiempo estos jugadores permanecen en un vector singleton de la clase Jugador
 *
 * @return true si pudo insertar TODOS los jugadores
 */
bool Inicio::insertPlayersInDB()
{
    QMap <QString, QString> players;
    for(int i = 0; i < ui->gridLayout->count();i++){
        RegisterPlayer * rp = qobject_cast<RegisterPlayer *>(ui->gridLayout->itemAt(i)->widget());
        if( ! (rp == 0) ){
//            qDebug() << "widget distinto de null";

            // obtengo foto de perfil
            QString foto_perfil = rp->getPathFotoPerfil();
            QString fp(foto_perfil);
            foto_perfil.push_back("'");
            foto_perfil.push_front("'");

            // obtengo nombre jugador
            QString nom_jugador = rp->getUserName();
            QString nom(nom_jugador);
            nom_jugador.push_back("'");
            nom_jugador.push_front("'");

            players["nom_jugador"] = nom_jugador;
            players["foto_perfil"] = foto_perfil;

            // inserto jugador en la bd, el id se genera solo
            if(! (Database::getInstance()->insert_into("jugadores", players)) )
                return false;

            // creo nuevo jugador y lo inserto en los jugadores actuales
            Jugador * jug = Factory::getJugador();
            jug->setFoto_perfil(fp);
            jug->setNombre(nom);
            // consultar el id del jugador y setearselo
            int nro_jug = Database::getInstance()->getLastRow("jugadores", "nro_jugador");
            qDebug() << "ultimo jug:" << nro_jug;
            jug->setNro_jugador(nro_jug);

            Jugador::getJugadoresActuales()->append(jug);
        }
    }

    return true;
}

/**
 * @brief Inicio::insertFichasJugadoresInDB por cada jugador(solo para 2 esta pensado)
 * se, relacionan los id de marcadores (3 para c/u y son siempre los mismos ids)
 * y se insertan en la bd
 * @return true si TODOS se insertaron, sino false.
 */
bool Inicio::insertVinculosInDB()
{
    qDebug() << "insertVinculosJugadoresDB";
    /* TODO:
     * una vez registrados los jugadores, debo asociarlos con un(os)
     * marcadores. para ello, tengo que insertar en la tabla
     * fichas_jugador el id del marcador y el nro de jugador
     *
     * Status: terminado. Pero podria mejorarse
     *
     * VERIFICAR: el marker id va a corresponder con el de la tabla ? si,
     * porque si estan todas las fichas, siempre va a corresponder
     * con alguno
    */

    QMap <QString, QString> mapFichas_Jugador;
    QVector<Jugador*> *vp = Jugador::getJugadoresActuales();

    /* dejo asociado 1 marker a cada jugador con estos id. en el futuro (ahre)
     * deberia tener minimo un vector o levantar los id de cada marker de algun
     * lado. y hacer un ciclo dentro de otro, poniendo los jugadores con los ids
     * que correspondan.
     *
     * Status: terminado
     */

    // cargo los valores por defecto del jugador 1 y 2
    QVector<int> jf1 = { 106, 111, 112 };
    QVector<int> jf2 = { 17, 20, 21 };
    QVector< QVector<int> > vi;
    vi.append(jf1);
    vi.append(jf2);

    // agrego los id por defecto de cada jugador
    for(int i = 0; i < vp->size(); i++)
        for(int j = 0; j < 3; j++){
            vp->at(i)->getVecids()->append( vi.at(i).at(j) );
        }


    // inserto en la tabla vinculos, los ids de los marcadores y su relacion con el jugador
    for(int i = 0; i < vp->size(); i++){
        Jugador * jug = vp->at(i);
        QStringList vinculos;

        for(int j = 0; j < jug->getVecids()->size(); j++){

            vinculos.clear();
            vinculos.append( QString::number( jug->getVecids()->at(j)) ); // marker_id
            vinculos.append( QString::number( jug->getNro_jugador()) );   // nro_jugador
            vinculos.append("null"); // recurso
            vinculos.append("null"); // formato caja

            if(!Database::getInstance()->insert_into("vinculos", vinculos)){
                qDebug() << "bool Inicio::insertVinculosInDB(): No se pudo insertar en tabla vinculos";
                return false;
            }
        }
    }

    return true;
}


void Inicio::slot_start(bool push)
{
    if (!registrarJugadores()) {
        qDebug() << "ERROR: Inicio::slot_start no se pudieron registrar todos los jugadores";
        this->close();
        return;
    }

    qDebug() << "slot_start, se pulso button";
    emit sig_start(this->getCurrentFJ());
}



//---------------------- Getters and setters -----------------------------


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

QVector<Jugador *> Inicio::getJugadoresActuales() const
{
    return jugadoresActuales;
}

void Inicio::setJugadoresActuales(const QVector<Jugador *> &value)
{
    jugadoresActuales = value;
}

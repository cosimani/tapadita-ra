#include "juego.h"
#include "ui_juego.h"

/* Junior del futuro:
 *
 * branch: interfaz
 *
 * TODO:
 *
 * 1- verificar que funciona el metodo initJuego y se carga todo lo de la carperta images
 * 2- cambiar que ahora se muestre el widget juego en vez de principal
 * 3- agregarle puntaje a groupbox de la vista juego (hacer el sistema de puntajes)
 */

Juego::Juego(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Juego)
{
    ui->setupUi(this);

    initJuego();
}

Juego::~Juego()
{
    delete ui;
}

void Juego::initJuego()
{
    QVector<Jugador*> *vp = Jugador::getJugadoresActuales();
    for( int i = 0; i < vp->size(); i++)
        ui->scene->addTexture(vp->at(i)->getFoto_perfil());

#ifdef PORTABLE
    QDir directory( "./Textures" );
#else
    QDir directory( APPLICATION_PATH "images" );
#endif

    QStringList fileFilter;
    fileFilter << "*.jpg" << "*.png" << "*.bmp" << "*.gif";
    QStringList imageFiles = directory.entryList( fileFilter );

    for ( int i = 0; i < imageFiles.size(); i++ )
    {
        QString textureUri = APPLICATION_PATH "images/" + imageFiles.at( i );
        ui->scene->addTexture(textureUri);
    }
    ui->scene->getSceneTimer()->start(10);
}

#include "juego.h"
#include "ui_juego.h"

#include "ui_settings.h"

Juego::Juego(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Juego),
    settings( new Settings() )
{
    ui->setupUi(this);

    // Para estirar la columna 0, que es la escena
    ui->gridLayout->setColumnStretch( 0, 1 );
    this->setFont( QFont( "Tahoma", 8 ) );
    cargarCamaras();

    puntajes.append(ui->puntaje1);
    puntajes.append(ui->puntaje2);

    // configuracion de settings
    ui->gbSettings->setMinimumHeight(this->width()/3);
    settings->ui->sSize->setRange(0, 100);
    slot_MarkerSize(settings->ui->sSize->value());


    connect(ui->pbSettings, SIGNAL(clicked(bool)), this, SLOT(slot_Settings()));
    connect(settings, SIGNAL(sig_ChangedMarkerSize(int)), this, SLOT(slot_MarkerSize(int)));
    connect(settings, SIGNAL(sig_ChangedProfilePicture(QString, int)), this, SLOT(slot_newProfilePicture(QString, int)));
}

Juego::~Juego()
{
    delete ui;
}

void Juego::initJuego()
{
    QVector<Jugador*> *vp = Jugador::getJugadoresActuales();
    for( int i = 0; i < vp->size(); i++){
        ui->scene->addTexture(vp->at(i)->getFoto_perfil());
        ui->scene->addTexture(vp->at(i)->getFoto_triangulacion());
        ui->scene->addTexture(vp->at(i)->getFoto_muerte());
    }

    // esto no funciona, por eso no lo uso.
/*
#ifdef PORTABLE
    QDir directory( "./Textures" );
#else
    QDir directory( APPLICATION_PATH "images" );
#endif

    qDebug() << "Actual path:" << QDir::currentPath();

    QStringList fileFilter;
    fileFilter << "*.png" << "*.jpg" << "*.gif" << "*.bmp";
    QStringList imageFiles = directory.entryList( fileFilter );
    qDebug() << "size:" << imageFiles.size();

    for ( int i = 0; i < imageFiles.size(); i++ )
    {
        QString textureUri = APPLICATION_PATH "images" + imageFiles.at( i );
        ui->scene->addTexture(textureUri);
    }
//*/

    ui->scene->getSceneTimer()->start(10);
}

/**
 * @brief Principal::cargarCamaras metodo que busca las camaras disponibles
 * y las enumera en cbCamara (checkbox)
 */
void Juego::cargarCamaras()
{

#ifdef CAMARAOPENCV

    /* NO FUNCIONA BIEN:
         * solo entra la primera vez al if cap->isOpen,
         * no puede soltar el cap, entonces no puede crear nuevos
         * VideoCapture porque esta ocupado el objeto
        */
        int counter = 0;
        for (int i = 0 ; i < 10 ; i++)  {
        VideoCapture * cap = new VideoCapture(i);
        qDebug() << cap->isOpened();
        if ( cap->isOpened() )  {
            cap->release();
//                ui->cbCamaras->addItem("camara numero: " + QString::number(i));
                counter++;

                if ( i == 1 )
//                    ui->cbCamaras->setEnabled(false);

                if ( counter > 0 )  {
//                    ui->cbCamaras->setCurrentIndex(1);
                    ui->scene->slot_cambiarCamara(1);
                }
            }
        }

        if ( counter == 0 )  {
            // en linea sig salt ex: "index out of range" de la clase QVector.
            QMessageBox::critical(this, "No se detectan camaras", "Controle que las camaras esten configuradas.");
            this->close();
        }

#else

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    qDebug() << cameras;

    for (int i=0 ; i<cameras.size() ; i++)
        ui->cbCamaras->addItem(cameras.at(i).description());

    if ( cameras.size() == 1 )
        ui->cbCamaras->setEnabled(false);

    if ( cameras.size() > 0 )  {
        ui->cbCamaras->setCurrentIndex(0);
        ui->scene->slot_cambiarCamara(0);
    }

    if ( cameras.size() == 0 )  {
        QMessageBox::critical(this, "No se detectan camaras", "Controle que las camaras estan configuradas.");
        this->close();
    }

#endif
}


void Juego::slot_Settings()
{
    qDebug() << "void Juego::slot_Settings()";

    // setear nombre de los jugadores en los botones
    settings->ui->pbPlayer1->setText(ui->puntaje1->getPlayerName());
    settings->ui->pbPlayer2->setText(ui->puntaje2->getPlayerName());

    // settear el valor del slider
    settings->setMarkerSizeValue( settings->ui->sSize->value() );



    settings->show();
}

void Juego::slot_MarkerSize(int size)
{
    qDebug() << "size:" << size;
    int oldMax = settings->ui->sSize->maximum();
    int oldMin = settings->ui->sSize->minimum();
    int oldRange = oldMax - oldMin;
    int newRange = MAXMARKERSIZE - MINMARKERSIZE;
    int newValue = ((( size - oldMin ) * newRange) / oldRange) + 50;

    ui->scene->setMarkerSize(newValue);
}

void Juego::slot_newProfilePicture(QString newPath, int player)
{
    QVector<Jugador*> *vp = Jugador::getJugadoresActuales();
    vp->at(player - 1)->setFoto_perfil(newPath);
    ui->scene->addTexture(vp->at(player - 1)->getFoto_perfil());
}

void Juego::slot_cbCamarasChanged(int nuevoIndex)
{
    ui->scene->slot_cambiarCamara(nuevoIndex);
}

QVector<Puntaje *> Juego::getPuntajes() const
{
    return puntajes;
}

void Juego::setPuntajes(const QVector<Puntaje *> &value)
{
    puntajes = value;
}

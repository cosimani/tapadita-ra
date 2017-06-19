#include "principal.h"
#include "ui_principal.h"
#include <QKeyEvent>
#include <QCameraInfo>
#include <QMessageBox>
#include "view/viewcontroller.h"
#include "model/jugador.h"


Principal::Principal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Principal)
{
    ui->setupUi(this);

    // Para estirar la columna 0, que es la escena
    ui->principalLayout->setColumnStretch( 0, 1 );

    this->setFont( QFont( "Tahoma", 8 ) );

    connect(ui->scene, SIGNAL(message(QString)), ui->teMessages, SLOT(append(QString)));
    connect(ui->rotateCheckBox, SIGNAL(clicked(bool)), ui->rotationVelocitySlider, SLOT(setEnabled(bool)));
    connect(ui->cbCamaras, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbCamarasChanged(int)));
    connect(ui->pbVincular, SIGNAL(clicked(bool)), this, SLOT(slot_vincular()));
    connect(this, SIGNAL(signal_vincular(int,QString,QString)), ui->scene, SLOT(slot_vincular(int,QString,QString)));


    this->cargarCamaras();
}

Principal::~Principal()
{
    delete ui;
}

void Principal::setVisibleSliders(bool visible)  {
    ui->teMessages->setVisible(visible);
    ui->appLogo->setVisible(visible);
    ui->markersGroup->setVisible(visible);
    ui->groupBox->setVisible(visible);
    ui->cbCamaras->setVisible(visible);
}


void Principal::initPrincipal()
{ 
    QVector<Jugador*> *vp = Jugador::getJugadoresActuales();
    for( int i = 0; i < vp->size(); i++){
        ui->scene->addTexture(vp->at(i)->getFoto_perfil());
//        qDebug() << "foto:" << vp->at(i)->getFoto_perfil();
//        qDebug() << "nombre:" << vp->at(i)->getNombre();
    }

    /* agrego la imagen del marker especial a mano, pero deberia
       consultar en la bd todos los especiales (podrian ser los
       que no tienen recurso de la tabla vinculos en null).
    */
    QString targetPath("/home/jrjs/obj.png");
    ui->scene->addTexture(targetPath);

    // cuando se llama a este metodo, comienza a estar visible esta clase
    ui->scene->getSceneTimer()->start(10);
}

/**
 * @brief Principal::cargarCamaras metodo que busca las camaras disponibles
 * y las enumera en cbCamara (checkbox)
 */
void Principal::cargarCamaras()
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
                ui->cbCamaras->addItem("camara numero: " + QString::number(i));
                counter++;

                if ( i == 1 )
                    ui->cbCamaras->setEnabled(false);

                if ( counter > 0 )  {
                    ui->cbCamaras->setCurrentIndex(1);
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


void Principal::slot_cbCamarasChanged(int nuevoIndex)
{
    ui->scene->slot_cambiarCamara(nuevoIndex);
}

void Principal::slot_vincular()
{
    if ( ui->cbModelos->currentIndex() != 0 )  {
        emit signal_vincular( ui->leMarcadorId->text().toInt(), ui->cbModelos->currentText(), "n" );
    }
    else  {
        if ( ui->cbFormatoCaja->isChecked() )
            emit signal_vincular( ui->leMarcadorId->text().toInt(), ui->leRecurso->text(), "s" );
        else
            emit signal_vincular( ui->leMarcadorId->text().toInt(), ui->leRecurso->text(), "n" );
    }

    ui->leRecurso->clear();
    ui->cbFormatoCaja->setCheckState(Qt::Unchecked);
    ui->scene->setFocus();
    ui->scene->actualizarTexturas();
    ui->cbModelos->setCurrentIndex(0);
}




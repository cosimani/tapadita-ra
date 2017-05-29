#include "principal.h"
#include "ui_principal.h"
#include <QKeyEvent>
#include <QCameraInfo>
#include <QMessageBox>

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

void Principal::cargarCamaras()
{
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
        QMessageBox::critical(this, "No se detectan cámaras", "Controle que las cámaras estén configuradas.");
        this->close();
    }
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



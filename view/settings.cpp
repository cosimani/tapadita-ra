#include "settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    ui->sSize->setValue(INITIAL_MARKER_SIZE);
    connect(ui->sSize, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderSize(int)));
    connect(ui->pbPlayer1, SIGNAL(sig_imagePath(QString)), this, SLOT(slot_changedProfilePicture1(QString)));
    connect(ui->pbPlayer2, SIGNAL(sig_imagePath(QString)), this, SLOT(slot_changedProfilePicture2(QString)));
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setMarkerSizeValue(int value)
{
    this->ui->sSize->setValue(value);
    this->ui->lSize->setText(QString::number(value) + "%");
}

void Settings::slot_sliderSize(int size)
{
    setMarkerSizeValue(size);
    emit sig_ChangedMarkerSize(size);
}

void Settings::slot_changedProfilePicture1(QString path)
{
    qDebug() << "path:" << path;
    emit sig_ChangedProfilePicture(path, 1);
}

void Settings::slot_changedProfilePicture2(QString path)
{
    qDebug() << "path:" << path;
    emit sig_ChangedProfilePicture(path, 2);
}

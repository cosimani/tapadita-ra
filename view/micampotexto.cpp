#include "micampotexto.h"
#include "ui_micampotexto.h"

MiCampoTexto::MiCampoTexto(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiCampoTexto)
{
    ui->setupUi(this);
}

MiCampoTexto::~MiCampoTexto()
{
    delete ui;
}

void MiCampoTexto::setTextToLabel(QString text)
{
    ui->label->setText(text);
}

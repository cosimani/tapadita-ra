#include "micampotexto.h"
#include "ui_micampotexto.h"

MiCampoTexto::MiCampoTexto(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiCampoTexto),
    userName("")
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

void MiCampoTexto::setTextToFild(QString text)
{
    ui->le->setText(text);
}

//--------------------- getters and setters--------------------
QString MiCampoTexto::getUserName()
{
    userName = ui->le->text();
    return userName;
}

void MiCampoTexto::setUserName(const QString &value)
{
    userName = value;
    ui->le->setText(userName);
}

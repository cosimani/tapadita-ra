#ifndef MICAMPOTEXTO_H
#define MICAMPOTEXTO_H

#include <QWidget>

namespace Ui {
class MiCampoTexto;
}

class MiCampoTexto : public QWidget
{
    Q_OBJECT

public:
    explicit MiCampoTexto(QWidget *parent = 0);
    ~MiCampoTexto();

    void setTextToLabel(QString text);

private:
    Ui::MiCampoTexto *ui;
};

#endif // MICAMPOTEXTO_H

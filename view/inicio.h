#ifndef INICIO_H
#define INICIO_H

#include <QWidget>

namespace Ui {
class Inicio;
}

class Inicio : public QWidget
{
    Q_OBJECT

public:
    explicit Inicio(QWidget *parent = 0);
    ~Inicio();

    /**
     * @brief se llama al crearse el objeto, para setear valores a las imagenes, label, y buton por defecto
     */
    void initDefaultValues();

private:
    Ui::Inicio *ui;

private slots:
    void slot_start(bool push);

signals:
    void sig_start();
};

#endif // INICIO_H

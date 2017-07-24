#ifndef PUNTAJE_H
#define PUNTAJE_H

#include <QWidget>
#include <QString>

namespace Ui {
class Puntaje;
}

class Puntaje : public QWidget
{
    Q_OBJECT

public:
    explicit Puntaje(QWidget *parent = 0);
    ~Puntaje();

    void updatePuntos(int puntos);
    void setPuntajeName(QString name);

private:
    Ui::Puntaje *ui;
};

#endif // PUNTAJE_H

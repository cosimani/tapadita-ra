#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <QWidget>
#include <QMap>
#include <QString>

namespace Ui  {
    class Principal;
}

class Principal : public QWidget  {
    Q_OBJECT

    QMap<QString, QString> fichas_jugadores;

public:
    explicit Principal(QWidget *parent = 0);
    ~Principal();
    void setVisibleSliders(bool visible);

    Ui::Principal *ui;  // Lo hice publico para poder acceder al ui desde Scene

    /**
     * @brief startTimer se llama desde ViewController para activar el timer de scene
     */
    void startTimer();

    // se setean desde viewcontroller cuando se pone visible esta clase
    void setFichas_jugadores(QMap<QString, QString> fichas_jugadores);
    QMap<QString, QString> getFichas_jugadores() const;

private:
    void cargarCamaras();

private slots:
    void slot_cbCamarasChanged(int nuevoIndex);
    void slot_vincular();

signals:
    void signal_vincular( int, QString, QString );
};

#endif // PRINCIPAL_H

#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#define CANTIDAD_MARCADORES 64

#include <QWidget>
#include "model/database.hpp"

namespace Ui {
class ViewController;
}

class ViewController : public QWidget
{
    Q_OBJECT

public:
    explicit ViewController(QWidget *parent = 0);
    ~ViewController();

private:
    Ui::ViewController *ui;

    /**
     * @brief se llama al crearse el objeto, para setear valores por defecto
     */
    void initDefaultValues();
    void initDataBase();

private slots:
    void slot_showPrincipal();

signals:
    void sig_startSceneTimer();

};

#endif // VIEWCONTROLLER_H

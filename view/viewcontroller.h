#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QWidget>

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

private slots:
    void slot_showPrincipal();

signals:
    void sig_startSceneTimer();

};

#endif // VIEWCONTROLLER_H

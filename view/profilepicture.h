#ifndef PROFILEPICTURE_H
#define PROFILEPICTURE_H

#include <QWidget>

namespace Ui {
class ProfilePicture;
}

class ProfilePicture : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePicture(QWidget *parent = 0);
    ~ProfilePicture();

private:
    Ui::ProfilePicture *ui;

    /**
     * @brief se llama al crearse el objeto, para setear valores a las imagenes, label, y buton por defecto
     */
    void initDefaultValues();

private slots:
    void slot_loadImage(QString path);
};

#endif // PROFILEPICTURE_H

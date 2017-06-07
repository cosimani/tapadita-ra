#ifndef PROFILEPICTURE_H
#define PROFILEPICTURE_H

#include <QWidget>
#include <QString>

namespace Ui {
class ProfilePicture;
}

class ProfilePicture : public QWidget
{
    Q_OBJECT

    QString pathFotoPerfil;

public:
    explicit ProfilePicture(QWidget *parent = 0);
    ~ProfilePicture();

    // getters and setters
    QString getPathFoto_perfil() const;
    void setPathFoto_perfil(const QString &value);

private:
    Ui::ProfilePicture *ui;

    /**
     * @brief se llama al crearse el objeto, para setear valores a las imagenes, label, y buton por defecto
     */
    void initDefaultValues();

private slots:
    void slot_loadImage(QString pathFotoPerfil);
};

#endif // PROFILEPICTURE_H

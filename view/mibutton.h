#ifndef MIBUTTON_H
#define MIBUTTON_H

#include <QPushButton>
#include <QFileDialog>

class MiButton : public QPushButton
{
    Q_OBJECT

    /**
     * @brief path de la imagen seleccionada
     */
    QString profilePicturePath;

public:
    MiButton(QWidget * parent = 0);
    virtual ~MiButton() {}

protected:
    void mousePressEvent(QMouseEvent *);

signals:

    /**
     * @brief señal que emite la ruta de la imagen seleccionada
     */
    void sig_imagePath(QString);
};

#endif // MIBUTTON_H

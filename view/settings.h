#ifndef SETTINGS_H
#define SETTINGS_H

#define INITIAL_MARKER_SIZE 64

#include <QDebug>
#include <QWidget>
#include <QString>
#include "ui_settings.h"

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    Ui::Settings *ui; // para que se pueda acceder desde Juego

    void setMarkerSizeValue(int value);

private slots:

    void slot_sliderSize(int size);
    void slot_changedProfilePicture1(QString path);
    void slot_changedProfilePicture2(QString path);

signals:
    void sig_ChangedMarkerSize(int size);
    void sig_ChangedProfilePicture(QString path, int player);

};

#endif // SETTINGS_H

#ifndef REGISTERPLAYER_H
#define REGISTERPLAYER_H

#include <QWidget>

namespace Ui {
class RegisterPlayer;
}

class RegisterPlayer : public QWidget
{
    Q_OBJECT

    QString pathFotoPerfil;
    QString userName;

public:
    explicit RegisterPlayer(QWidget *parent = 0);
    ~RegisterPlayer();

    QString getPathFotoPerfil();
    void setPathFotoPerfil(const QString &value);

    QString getUserName();
    void setUserName(const QString &value);

private:
    Ui::RegisterPlayer *ui;
};

#endif // REGISTERPLAYER_H

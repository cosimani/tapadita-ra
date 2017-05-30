#ifndef REGISTERPLAYER_H
#define REGISTERPLAYER_H

#include <QWidget>

namespace Ui {
class RegisterPlayer;
}

class RegisterPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterPlayer(QWidget *parent = 0);
    ~RegisterPlayer();

private:
    Ui::RegisterPlayer *ui;
};

#endif // REGISTERPLAYER_H

#ifndef MICAMPOTEXTO_H
#define MICAMPOTEXTO_H

#include <QWidget>

namespace Ui {
class MiCampoTexto;
}

class MiCampoTexto : public QWidget
{
    Q_OBJECT

    QString userName;

public:
    explicit MiCampoTexto(QWidget *parent = 0);
    ~MiCampoTexto();

    void setTextToLabel(QString text);
    void setTextToFild(QString text);

    QString getUserName();
    void setUserName(const QString &value);

private:
    Ui::MiCampoTexto *ui;
};

#endif // MICAMPOTEXTO_H

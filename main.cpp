#include <QApplication>
#include "principal.h"
#include "view/viewcontroller.h"
#include "view/inicio.h"

int main( int argc, char **argv )  {
    QApplication app( argc, argv );

    ViewController principal;
//    Inicio principal;
    principal.showMaximized();

    return app.exec();
}

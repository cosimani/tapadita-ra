#include <QApplication>
#include "view/principal.h"
#include "view/viewcontroller.h"
#include "view/inicio.h"

int main( int argc, char **argv )  {
    QApplication app( argc, argv );

    ViewController vp;
    vp.showMaximized();

    return app.exec();
}

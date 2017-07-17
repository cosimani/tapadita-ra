#ifndef FACTORY_H
#define FACTORY_H

#include "model/jugador.h"

class Factory
{
    Factory();

public:
    static Jugador* getJugador(){
        return new Jugador();
    }
};

#endif // FACTORY_H

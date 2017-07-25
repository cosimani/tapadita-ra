#ifndef TEXTURE_ES_H
#define TEXTURE_ES_H

#include<QString>
#include<QOpenGLTexture>

class Texture_ES {

public:
    QString name;
    QOpenGLTexture *tex;
    Texture_ES ( QString ruta = ""): name(ruta) {
        tex = new QOpenGLTexture (QImage(ruta).mirrored());
    }
};

#endif // TEXTURE_ES_H

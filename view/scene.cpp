#include "scene.h"
#include <QApplication>
#include "ui_principal.h"  // Necesario para acceder al ui de Principal
#include "model/qrcode.h"
#include "ui_juego.h"


#ifndef OPENGL_ES
Scene::Scene( QWidget *parent ) : QGLWidget( parent ),
#else
Scene::Scene(QWidget *parent) : QOpenGLWidget(parent),

#endif

                                  nCamera( 0 ),

                                  videoCapture ( new cv::VideoCapture( nCamera ) ),

                                  sceneTimer ( new QTimer ),
                                  milisecondsTimer( 10 ),

                                  textures( new QVector< Texture * > ),
                                  texturesVinculadas( new QVector< Texture * > ),
                                  models( new QVector< Model * > ),
                                  videos( new QVector< Video * > ),
                                  videosVinculados( new QVector< Video * > ),

                                  markerDetector( new MarkerDetector ),
                                  cameraParameters( new CameraParameters ),

                                  zRotationVelocity( 0 ),

                                  winnerLine(250),
                                  zonaTriangulacion(350),

//                                  markerSize(64),

                                  juego ( (Juego*) parent )
{
    // para que detecte las teclas cuando clickeo el widget o con tab
    this->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    videoCapture = new cv::VideoCapture();

//    QMessageBox::information(this, "es", "dasd"+QDir::currentPath());

#ifdef PORTABLE
    QFile file( "Files/CameraParameters.yml" );
    if ( !file.exists() )  {
        QMessageBox::critical(this, "No se puede configurar la camara", "Falta el archivo CameraParameters.yml.");
        std::exit(0);
    }
    cameraParameters->readFromXMLFile( "Files/CameraParameters.yml" );
#else
    QFile file( APPLICATION_PATH "Files/CameraParameters.yml" );
    if ( !file.exists() )  {
        QMessageBox::critical(this, "No se puede configurar la camara", "Falta el archivo CameraParameters.yml.");
        std::exit(0);
    }
    cameraParameters->readFromXMLFile( APPLICATION_PATH "Files/CameraParameters.yml" );
#endif

    if ( ! cameraParameters->isValid() )  {

    }

//    sceneTimer->start( milisecondsTimer );
    connect( sceneTimer, SIGNAL( timeout() ), SLOT( slot_updateScene() ) );

//    Database::getInstance()->checkBase();
#ifdef OPENGL_ES
    x = y = 0;
    z = 0;
    angulo = 0;
    vImageBuffer = new QVector< QOpenGLBuffer * >();
    vImageTexture = new QVector< Texture_ES * >();
#endif
}

Scene::~Scene()
{
    videoCapture->release();
}

void Scene::actualizarTexturas()
{
    texturesVinculadas->clear();
    videosVinculados->clear();

    imageFiles = Database::getInstance()->readVinculos();
    qDebug() << "imageFiles.size()" << imageFiles.size(); // 64, las que cargue en la bd.
    for ( int i = 0; i < imageFiles.size(); i++ )
    {
        QFileInfo fileInfo = imageFiles.at( i ).at(1);
        QString fileName = fileInfo.fileName();
        if ( fileInfo.suffix() != "mp4" )  {  // Entra si es png

            texturesVinculadas->append( new Texture( fileName ) );
            QString textureUri = imageFiles.at( i ).at(1);

            if(textureUri.size() > 0){
                qDebug() << "texture Uri" << textureUri;
                Mat textureMat = imread( textureUri.toStdString(), CV_LOAD_IMAGE_COLOR );
    //            cv::cvtColor(textureMat, textureMat, )
        //        flip( textureMat, textureMat, 0 );
                texturesVinculadas->last()->mat = textureMat;
                texturesVinculadas->last()->generateFromMat();
            }
        }
        else  {  // Entra si es mp4

//            videosVinculados->append( new Video( fileName ) );
            videosVinculados->append( new Video( fileInfo.absoluteFilePath() ) );
        }
    }


}

QTimer *Scene::getSceneTimer() const
{
    return sceneTimer;
}

void Scene::setSceneTimer(QTimer *value)
{
    sceneTimer = value;
}

int Scene::getMarkerSize() const
{
    return markerSize;
}

void Scene::setMarkerSize(int value)
{
    markerSize = value;
}

void Scene::loadTextures()
{
#ifdef OPENGL_ES
    QDir directory( "../tapadita-ra/images" );

    QStringList fileFilter;
        fileFilter << "*.jpg" << "*.png" << "*.bmp" << "*.gif";
        QStringList imageFiles = directory.entryList( fileFilter );        

        for ( int i = 0; i < imageFiles.size(); i++ )
        {
            QOpenGLBuffer * imageBuffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );

            imageBuffer->create();

            static const int coords[1][4][3] = { { { +1, -1, 0 }, { -1, -1, 0 }, { -1, 0, 0 }, { +1, 0, 0 } } };

            QVector<GLfloat> vertData;
            for (int i = 0; i < 1; ++i) {
                for (int j = 0; j < 4; ++j) {
                    // vertex position
                    vertData.append( coords[i][j][0]);
                    vertData.append( coords[i][j][1]);
                    vertData.append( coords[ i ][ j ][ 2 ]);

                    // texture coordinate
                    if ( j==0 )  {  vertData.append(1);  vertData.append(0);  }
                    if ( j==1 )  {  vertData.append(0);  vertData.append(0);  }
                    if ( j==2 )  {  vertData.append(0);  vertData.append(1);  }
                    if ( j==3 )  {  vertData.append(1);  vertData.append(1);  }
                }
            }

            if ( ! imageBuffer->bind() )
                QMessageBox::critical(this, "False", "vbo2 bind()");

            imageBuffer->allocate( vertData.constData(), vertData.count() * sizeof( GLfloat ) );


//            QOpenGLTexture * imageTexture = new QOpenGLTexture( QImage( "../Textures/" + imageFiles.at( i ) ) );
            Texture_ES * imageTexture = new Texture_ES( "../tapadita-ra/images/" + imageFiles.at( i ));
            imageTexture->tex->setMinificationFilter( QOpenGLTexture::Nearest );
            imageTexture->tex->setMagnificationFilter( QOpenGLTexture::Linear );
            qDebug()<<imageTexture->name;


        vImageBuffer->append( imageBuffer );
        vImageTexture->append( imageTexture );
        }
#else
    qDebug()<<"no se usa loadTextures() sin OpenGlES";
#endif
}

void Scene::addTexture(QString imagen){
#ifndef OPENGL_ES
    textures->append(new Texture(imagen));
    Mat textureMat = imread( imagen.toStdString() );
    flip( textureMat, textureMat, 0 );
    textures->last()->mat = textureMat;
    textures->last()->generateFromMat();
#else

    QOpenGLBuffer * imageBuffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );

    imageBuffer->create();

    static const int coords[1][4][3] = { { { +1, -1, 0 }, { -1, -1, 0 }, { -1, 0, 0 }, { +1, 0, 0 } } };

    QVector<GLfloat> vertData;
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append( coords[i][j][0]);
            vertData.append( coords[i][j][1]);
            vertData.append( coords[ i ][ j ][ 2 ]);

            // texture coordinate
            if ( j==0 )  {  vertData.append(1);  vertData.append(0);  }
            if ( j==1 )  {  vertData.append(0);  vertData.append(0);  }
            if ( j==2 )  {  vertData.append(0);  vertData.append(1);  }
            if ( j==3 )  {  vertData.append(1);  vertData.append(1);  }
        }
    }

    if ( ! imageBuffer->bind() )
        QMessageBox::critical(this, "False", "vbo2 bind()");

    imageBuffer->allocate( vertData.constData(), vertData.count() * sizeof( GLfloat ) );

    Texture_ES * imageTexture = new Texture_ES( imagen );
    imageTexture->tex->setMinificationFilter( QOpenGLTexture::Nearest );
    imageTexture->tex->setMagnificationFilter( QOpenGLTexture::Linear );
    qDebug()<<imageTexture->name;

    vImageBuffer->append( imageBuffer );
    vImageTexture->append( imageTexture );


#endif
}

void Scene::loadModels()
{
#ifdef PORTABLE
    QDir directory( "./Models" );
#else
    QDir directory( APPLICATION_PATH "Models" );
#endif

    QStringList fileFilter;
    fileFilter << "*.3ds";
    QStringList modelFiles = directory.entryList( fileFilter );

    for ( int i = 0 ; i < modelFiles.size() ; i++ )
        models->append( new Model( modelFiles.at( i ) ) );

    prepareModels();

}

void Scene::prepareModels()
{
    loadTexturesForModels();

    for ( int i = 0 ; i < models->size() ; i++)
    {
        if( !models->at( i ) ) return;

        models->at( i )->getFaces();
        Lib3dsVector *vertices = new Lib3dsVector[ models->at( i )->totalFaces * 3 ];
        Lib3dsVector *normals = new Lib3dsVector[ models->at( i )->totalFaces * 3 ];
        Lib3dsTexel *texCoords = new Lib3dsTexel[ models->at( i )->totalFaces * 3 ];
        Lib3dsMesh *mesh;

        unsigned int finishedFaces = 0;

        for( mesh = models->at(i)->model->meshes; mesh != NULL ; mesh = mesh->next )
        {
            lib3ds_mesh_calculate_normals( mesh, &normals[ finishedFaces * 3 ] );
            for( unsigned int currentFace = 0; currentFace < mesh->faces ; currentFace++ )
            {
                Lib3dsFace * face = &mesh->faceL[ currentFace ];
                for( unsigned int i = 0; i < 3; i++ )
                {
                    if( &mesh->texelL )
                        memcpy( &texCoords[ finishedFaces*3 + i ],
                                mesh->texelL[ face->points[ i ] ],
                                sizeof( Lib3dsTexel ) );

                    memcpy( &vertices[ finishedFaces * 3 + i ],
                            mesh->pointL[ face->points[ i ] ].pos,
                            sizeof( Lib3dsVector ) );
                }
                finishedFaces++;
            }
        }

        glGenBuffers( 1, &models->at(i)->vertexVBO );
        glBindBuffer( GL_ARRAY_BUFFER, models->at(i)->vertexVBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( Lib3dsVector ) * 3 * models->at(i)->totalFaces, vertices, GL_STATIC_DRAW );

        glGenBuffers( 1, &models->at(i)->normalVBO );
        glBindBuffer( GL_ARRAY_BUFFER, models->at(i)->normalVBO);
        glBufferData( GL_ARRAY_BUFFER, sizeof( Lib3dsVector ) * 3 * models->at(i)->totalFaces, normals, GL_STATIC_DRAW );

        glGenBuffers( 1, &models->at(i)->texCoordVBO );
        glBindBuffer( GL_ARRAY_BUFFER, models->at(i)->texCoordVBO);
        glBufferData( GL_ARRAY_BUFFER, sizeof( Lib3dsTexel ) * 3 * models->at(i)->totalFaces, texCoords, GL_STATIC_DRAW );

        delete vertices;
        delete normals;
        delete texCoords;

        lib3ds_file_free( models->at(i)->model );
        models->at(i)->model = NULL;
    }
}

void Scene::loadTexturesForModels()
{
    for ( int i = 0 ; i < models->size(); i++ )
    {
        QString modelTextureName = models->at( i )->name;
        modelTextureName.remove( ".3ds" );
        modelTextureName += ".jpg";

        for( int j = 0; j < textures->size(); j++ )
            if( textures->at( j )->name == modelTextureName )
                models->operator []( i )->textureId = textures->at( j )->id;
    }
}

void Scene::loadVideos()
{
#ifdef PORTABLE
    QDir directory( "./Videos" );
#else
    QDir directory( APPLICATION_PATH "Videos" );
#endif

    QStringList fileFilter;
    fileFilter << "*.avi" << "*.wmv" << "*.mpg" << "*.mpeg" << "*.mpeg1" << "*.mpeg2" << "*.mpeg4" << "*.mp4";
    QStringList videoFiles = directory.entryList( fileFilter );

    for ( int i = 0 ; i < videoFiles.size() ; i++ )
        videos->append( new Video( videoFiles.at( i ) ) );
}

void Scene::initializeGL()
{
#ifndef OPENGL_ES
    initializeGLFunctions();
    GLfloat lightAmbient[4]; lightAmbient[0] = 0.5f;  lightAmbient[1] = 0.5f;
            lightAmbient[2] = 0.5f;  lightAmbient[3] = 1.0f;

    GLfloat lightDiffuse[4]; lightDiffuse[0] = 1.0f;  lightDiffuse[1] = 1.0f;
            lightDiffuse[2] = 1.0f;  lightDiffuse[3] = 1.0f;

    GLfloat lightPosition[4];lightPosition[0]= 0.0f;  lightPosition[1]= 0.0f;
            lightPosition[2]= 2.0f;  lightPosition[3]= 1.0f;

    glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );  glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse );
    glLightfv( GL_LIGHT1, GL_POSITION,lightPosition ); glEnable( GL_LIGHT1 );

    textures->append( new Texture( "CameraTexture" ) );

    glShadeModel( GL_SMOOTH );
#else
    initializeOpenGLFunctions();
    this->loadTextures();

    this->crearProgram();

    this->crearCam();
    geometries = new GeometryEngine;
#endif

    glClearColor( 0, 0, 0, 0 );

    glEnable( GL_DEPTH_TEST );    
}

void Scene::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );
}

void Scene::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
#ifndef OPENGL_ES
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( 0, RESOLUTION_WIDTH, 0, RESOLUTION_HEIGHT, 1, 1000 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Inicio: Gráfico de cámara

    glEnable( GL_TEXTURE_2D );
    glColor3f( 1, 1, 1 );
    glBindTexture( GL_TEXTURE_2D, textures->at( 0 )->id );
    glBegin( GL_QUADS );

        glTexCoord2f( 0, 0 ); glVertex3f( 0, RESOLUTION_HEIGHT, -999 );
        glTexCoord2f( 1, 0 ); glVertex3f( RESOLUTION_WIDTH, RESOLUTION_HEIGHT, -999 );
        glTexCoord2f( 1, 1 ); glVertex3f( RESOLUTION_WIDTH, 0, -999 );
        glTexCoord2f( 0, 1 ); glVertex3f( 0, 0, -999 );

    glEnd();


    glDisable( GL_TEXTURE_2D );

    // Fin: Gráfico de cámara

    glMatrixMode( GL_PROJECTION );
    double projectionMatrix[16];

    cv::Size2i sceneSize( RESOLUTION_WIDTH, RESOLUTION_HEIGHT );
    cv::Size2i openGlSize( RESOLUTION_WIDTH, RESOLUTION_HEIGHT );
    cameraParameters->glGetProjectionMatrix( sceneSize, openGlSize, projectionMatrix, 0.05, 10 );

    glLoadMatrixd( projectionMatrix );
    glMatrixMode( GL_MODELVIEW );
    double modelview_matrix[16];

    // Inicio: Gráfico en marcadores

    /* itero sobre la cantidad de jugadores, y por cada jugador, pregunto
     * si entre los markers detectados, el marker corresponde a ese jugador
     * en caso de ser cierto, dibujo la imagen de ese jugador
    */
//    float coefTamano = (float)principal->ui->sbTamano->value() / (float)100;
    float coefTamano = 1;
    QVector<Jugador*> * vp = Jugador::getJugadoresActuales();
    for(int i = 0; i < vp->size(); i++){

        Jugador * jug = vp->at(i);
        QString fp = jug->getFoto_perfil();

        for( int j = 0 ; j < detectedMarkers.size() ; j++ ){

            // aca se toma la matrix de cada marcador
            detectedMarkers.operator []( j ).glGetModelViewMatrix( modelview_matrix );
            // y se setea la matrix de ese mmarkerSizearcador, para que la pueda dibujar y seguir
            glLoadMatrixd( modelview_matrix );


            //-------------------Inicio: dibujo imagen en marker ----------------------
            int mi = detectedMarkers.at(j).id;            
            // pregunto si el jugador tiene en su vector de ids, el
            // id del marcador encontrado y dibujo su foto si esta esa relacion.
            if( jug->getVecids()->indexOf(mi) != -1) {


                // tengo que ver a quien pertenece el id en mi y ver si esta visible el marker

                for(int k = 0; k < jug->getFichas()->size(); k++){
                    QRCode * qrc = jug->getFichas()->at(k);

                    if (qrc->getMkr().id == mi) {

                        switch (qrc->getEstado()) {
                        case QRCode::ESTADO::valid:
                            drawBox(fp, detectedMarkers.at( j ).ssize, markerSize);
                            qrc->updatePuntaje(100);
                            break;

                        case QRCode::ESTADO::dead:
                            drawBox(jug->getFoto_muerte(), detectedMarkers.at( j ).ssize, markerSize);
                            qrc->updatePuntaje(-50);
                            break;

                        case QRCode::ESTADO::triangulation:
                            drawBox(jug->getFoto_triangulacion(), detectedMarkers.at( j ).ssize, markerSize);
                            qrc->updatePuntaje(0);                            
                            break;

                        default:
                            break;
                        }
                    }
                }
            }

            //-------------------Fin: dibujo imagen en marker ----------------------
        }

        jug->updatePuntaje();
    }

    // Fin: Gráfico en marcadores
    glFlush();
#else
    // primero dibujo en el plano que contendra la imagen de la camara
    this->drawCamera();

    if( detectedMarkers.size() > 0 )  {        

        double projectionMatrix[16];
        cameraParameters->glGetProjectionMatrix( cv::Size2i( 640, 480 ),
                                                 cv::Size2i( 640, 480 ),
                                                 projectionMatrix, 0.05, 10 );
        // Inicio: Gráfico en marcadores

        /* itero sobre la cantidad de jugadores, y por cada jugador, pregunto
         * si entre los markers detectados, el marker corresponde a ese jugador
         * en caso de ser cierto, dibujo la imagen de ese jugador
        */
        float coefTamano = 1;

        QVector<Jugador*> * vp = Jugador::getJugadoresActuales();
        for(int i = 0; i < vp->size(); i++){

                Jugador * jug = vp->at(i);
                QString fp = jug->getFoto_perfil();
                for( unsigned int j = 0 ; j < detectedMarkers.size() ; j++ )
                {
                    double modelview_matrix[ 16 ];
                    detectedMarkers.operator []( j ).glGetModelViewMatrix( modelview_matrix );

                    QMatrix4x4 projection;
                    QMatrix4x4 modelView;

                    projection( 0, 0 ) = projectionMatrix[ 0 ];
                    projection( 1, 0 ) = projectionMatrix[ 1 ];
                    projection( 2, 0 ) = projectionMatrix[ 2 ];
                    projection( 3, 0 ) = projectionMatrix[ 3 ];
                    projection( 0, 1 ) = projectionMatrix[ 4 ];
                    projection( 1, 1 ) = projectionMatrix[ 5 ];
                    projection( 2, 1 ) = projectionMatrix[ 6 ];
                    projection( 3, 1 ) = projectionMatrix[ 7 ];
                    projection( 0, 2 ) = projectionMatrix[ 8 ];
                    projection( 1, 2 ) = projectionMatrix[ 9 ];
                    projection( 2, 2 ) = projectionMatrix[ 10 ];
                    projection( 3, 2 ) = projectionMatrix[ 11 ];
                    projection( 0, 3 ) = projectionMatrix[ 12 ];
                    projection( 1, 3 ) = projectionMatrix[ 13 ];
                    projection( 2, 3 ) = projectionMatrix[ 14 ];
                    projection( 3, 3 ) = projectionMatrix[ 15 ];

                    modelView( 0, 0 ) = modelview_matrix[ 0 ];
                    modelView( 1, 0 ) = modelview_matrix[ 1 ];
                    modelView( 2, 0 ) = modelview_matrix[ 2 ];
                    modelView( 3, 0 ) = modelview_matrix[ 3 ];
                    modelView( 0, 1 ) = modelview_matrix[ 4 ];
                    modelView( 1, 1 ) = modelview_matrix[ 5 ];
                    modelView( 2, 1 ) = modelview_matrix[ 6 ];
                    modelView( 3, 1 ) = modelview_matrix[ 7 ];
                    modelView( 0, 2 ) = modelview_matrix[ 8 ];
                    modelView( 1, 2 ) = modelview_matrix[ 9 ];
                    modelView( 2, 2 ) = modelview_matrix[ 10 ];
                    modelView( 3, 2 ) = modelview_matrix[ 11 ];
                    modelView( 0, 3 ) = modelview_matrix[ 12 ];
                    modelView( 1, 3 ) = modelview_matrix[ 13 ];
                    modelView( 2, 3 ) = modelview_matrix[ 14 ];
                    modelView( 3, 3 ) = modelview_matrix[ 15 ];

//                    modelView.scale( 0.10 );
                    modelView.scale( 0.20 );
                    modelView.translate( x, y, z );
                    modelView.rotate( angulo, 0, 0, 1 );

                    program->setUniformValue( "projection", projection * modelView );


                    //-------------------Inicio: dibujo imagen en marker ----------------------
                    int mi = detectedMarkers.at(j).id;
                    // pregunto si el jugador tiene en su vector de ids, el
                    // id del marcador encontrado y dibujo su foto si esta esa relacion.
                    if( jug->getVecids()->indexOf(mi) != -1) {

                        // tengo que ver a quien pertenece el id en mi y ver si esta visible el marker

                        for(int k = 0; k < jug->getFichas()->size(); k++){
                            QRCode * qrc = jug->getFichas()->at(k);

                            if (qrc->getMkr().id == mi) {

                                switch (qrc->getEstado()) {
                                case QRCode::ESTADO::valid:
                                    drawSheet(fp, detectedMarkers.at( j ).ssize, 100 * coefTamano);
                                    break;

                                case QRCode::ESTADO::dead:                                    
                                    drawSheet(jug->getFoto_muerte(), detectedMarkers.at( j ).ssize, 100 * coefTamano);
                                    break;

                                case QRCode::ESTADO::triangulation:
                                    drawSheet(jug->getFoto_triangulacion(), detectedMarkers.at( j ).ssize, 100 * coefTamano);
                                    break;

                                default:
                                    break;
                                }
                            }
                        }
                    }                    
                }
            }
    }
#endif
}

void Scene::keyPressEvent( QKeyEvent *event )
{

    switch( event->key() )  {

    case Qt::Key_M:
        if ( juego->isFullScreen() )  {
            juego->showMaximized();
        }
        else  {
            juego->showFullScreen();;
        }
        break;

    case Qt::Key_Escape:
        qApp->quit();
        break;

    case Qt::Key_Up:
        if (juego->ui->cbWinnerLine->isChecked() && winnerLine > 0 && winnerLine < RESOLUTION_HEIGHT)
            winnerLine -= 1;
        if (juego->ui->cbZonaTri->isChecked())
            zonaTriangulacion -= 1;
        break;

    case Qt::Key_Down:
        if (juego->ui->cbWinnerLine->isChecked() && zonaTriangulacion > 0 && zonaTriangulacion < RESOLUTION_HEIGHT)
            winnerLine += 1;
        if (juego->ui->cbZonaTri->isChecked())
            zonaTriangulacion += 1;
        break;


    default:;
    }
}

void Scene::drawCameraBox(unsigned int percentage )
{
    qDebug() << "$$$$$$$$$$$$$$$$$$$$ drawCameraBox $$$$$$$$$$$$$$$$$$$$$$$";
    drawBox( "CameraTexture", percentage );
}

/**
 * @brief Scene::drawSheet Dibuja una imagen. (Ver descripcion de Scene::drawBox para mas informacion)
 */
void Scene::drawSheetVinculadas(QString textureName, float sizeMarker, unsigned int percentage )
{

    qDebug() << "################### drawSheetVinculadas #######################";
    sizeMarker = sizeMarker * (float)percentage / (float)100;

    for( int i = 0; i < texturesVinculadas->size(); i++ )
    {
        if( texturesVinculadas->at( i )->name == textureName )
        {
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, texturesVinculadas->at( i )->id );
            glColor3f( 1, 1, 1 );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f,-1.0f);
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );

            glEnd();
            glDisable( GL_TEXTURE_2D);
        }
    }
}

/**
 * @brief Scene::drawSheet Dibuja una imagen. (Ver descripcion de Scene::drawBox para mas informacion)
 */
void Scene::drawSheet(QString textureName, float sizeMarker, unsigned int percentage )
{
    #ifndef OPENGL_ES
//    qDebug() << "<<<<<<<<<<<<<<<<< drawSheet <<<<<<<<<<<<<<<<<<<<<<<<<";
    sizeMarker = sizeMarker * (float)percentage / (float)100;
    for( int i = 0; i < textures->size(); i++ )
    {
        if( textures->at( i )->name == textureName )
        {            
//            qDebug() << "------------drawSheet entra a dibujar------------";
            qDebug()<<textureName;
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, textures->at( i )->id );
            glColor3f( 1, 1, 1 );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f,-1.0f);
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );

            glEnd();
            glDisable( GL_TEXTURE_2D);
        }
    }
#else
    sizeMarker = sizeMarker * (float)percentage / (float)100;

    for( int i = 0; i < vImageTexture->size(); i++ )
    {
        qDebug()<<"si: " + vImageTexture->at( i )->name +" = "+ textureName;
        if( vImageTexture->at( i )->name == textureName )
        {
            qDebug()<<"Dibujo: " + textureName;
            vImageTexture->at( i )->tex->bind(0);
            vImageBuffer->at( i )->bind();
//            geometries->drawSheetGeometry( program );
            geometries->drawCubeGeometry( program );
            break;
        }
    }
#endif
}

/**
 * @brief Scene::drawBox Dibuja una caja. Aruco entrega el punto (0,0,0) justo en el centro del marcador.
 * @param textureName Nombre del archivo de imagen en la carpeta Textures. Sera la textura de la caja.
 * @param sizeMarker Tamano del marcador que lo entrega Aruco en el atributo ssize de la clase Marker.
 * @param percentage Para redimensionar la caja. En escala 1:1 la caja tiene el tamano del marcador.
 */
void Scene::drawBox( QString textureName, float sizeMarker, unsigned int percentage )
{
//    qDebug() << """""""""""""""""drawBox""""""""""""""""""""""";
    sizeMarker = sizeMarker * (float)percentage / (float)100;

    for( int i = 0; i < textures->size(); i++ )
    {
//        qDebug()<<"si " + textures->at( i )->name + " es = " + textureName;
        if( textures->at( i )->name == textureName )
        {
//            qDebug()<<"entro a pintar: " + textureName;
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, textures->at( i )->id );
            glColor3f( 1, 1, 1 );
            glEnable( GL_LIGHTING );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f, 1.0f ); // Techo
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, sizeMarker );

                glNormal3f( 0.0f, 0.0f,-1.0f ); // Piso
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );

                glNormal3f( 0.0f, 1.0f, 0.0f ); // Trasera
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );

                glNormal3f( 0.0f,-1.0f, 0.0f ); // Frontal
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, sizeMarker );

                glNormal3f( 1.0f, 0.0f, 0.0f ); // Derecha
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, sizeMarker );

                glNormal3f( -1.0f, 0.0f, 0.0f ); // Izquierda
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );

            glEnd();
            glDisable( GL_LIGHTING );
            glDisable( GL_TEXTURE_2D );
        }
    }
}

/**
 * @brief Scene::drawBox Dibuja una caja. Aruco entrega el punto (0,0,0) justo en el centro del marcador.
 * @param textureName Nombre del archivo de imagen en la carpeta Textures. Sera la textura de la caja.
 * @param sizeMarker Tamano del marcador que lo entrega Aruco en el atributo ssize de la clase Marker.
 * @param percentage Para redimensionar la caja. En escala 1:1 la caja tiene el tamano del marcador.
 */
void Scene::drawBoxVinculado( QString textureName, float sizeMarker, unsigned int percentage )
{
    qDebug() << "------------drawBoxVinculado------------"; //
    sizeMarker = sizeMarker * (float)percentage / (float)100;

    for( int i = 0; i < texturesVinculadas->size(); i++ )
    {
        if( texturesVinculadas->at( i )->name == textureName )
        {
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, texturesVinculadas->at( i )->id );
            glColor3f( 1, 1, 1 );
            glEnable( GL_LIGHTING );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f, 1.0f ); // Techo
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, sizeMarker );

                glNormal3f( 0.0f, 0.0f,-1.0f ); // Piso
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );

                glNormal3f( 0.0f, 1.0f, 0.0f ); // Trasera
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );

                glNormal3f( 0.0f,-1.0f, 0.0f ); // Frontal
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, sizeMarker );

                glNormal3f( 1.0f, 0.0f, 0.0f ); // Derecha
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2, -sizeMarker/2, sizeMarker );

                glNormal3f( -1.0f, 0.0f, 0.0f ); // Izquierda
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2, -sizeMarker/2, sizeMarker );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, sizeMarker );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sizeMarker/2,  sizeMarker/2, 0 );

            glEnd();
            glDisable( GL_LIGHTING );
            glDisable( GL_TEXTURE_2D );
        }
    }
}

void Scene::drawModel( QString modelName, int percentage )
{
    qDebug() << "|||||||||||||drawModel||||||||||||||||";
    float scale = percentage / ( float )1000;
    for ( int i = 0 ; i < models->size(); i++ )
    {
        if ( models->at( i )->name == modelName )
        {
            if( !models->at( i )->totalFaces ) return;

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, models->at( i )->textureId );
            glScalef( scale, scale, -scale );
            glEnableClientState( GL_VERTEX_ARRAY );
            glEnableClientState( GL_NORMAL_ARRAY );
            glEnableClientState( GL_TEXTURE_COORD_ARRAY );

                glBindBuffer( GL_ARRAY_BUFFER, models->at( i )->normalVBO );
                glNormalPointer( GL_FLOAT, 0, NULL );
                glBindBuffer( GL_ARRAY_BUFFER, models->at( i )->texCoordVBO );
                glTexCoordPointer( 2, GL_FLOAT, 0, NULL );
                glBindBuffer( GL_ARRAY_BUFFER, models->at( i )->vertexVBO );
                glVertexPointer( 3, GL_FLOAT, 0, NULL );
                glDrawArrays( GL_TRIANGLES, 0, models->at( i )->totalFaces * 3 );

            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );

            glDisable( GL_TEXTURE_2D );
        }
    }
}

void Scene::drawVideo( QString videoName, float sizeMarker, unsigned int percentage, int volume )
{
    qDebug() << "&&&&&&&&&&&&&&&&&&& drawVideo &&&&&&&&&&&&&&&&&&&&&&";
    sizeMarker = sizeMarker * (float)percentage / (float)100;

    for ( int i = 0 ; i < videos->size(); i++ )
    {
        if ( videos->at( i )->name == videoName )
        {
            videos->at( i )->player->play();
            videos->at( i )->player->setVolume( volume );

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, videos->at( i )->grabber->textureId );
            glColor3f( 1, 1, 1 );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f,-1.0f);
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2*( 16 / ( float )9 ), -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2*( 16 / ( float )9 ),  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2*( 16 / ( float )9 ),  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2*( 16 / ( float )9 ), -sizeMarker/2, 0 );

            glEnd();
            glDisable( GL_TEXTURE_2D);
        }
    }
}

void Scene::drawVideoVinculado( QString videoName, float sizeMarker, unsigned int percentage, int volume )
{
    qDebug() << "(((((((((((((((((( drawVideoVinculado (((((((((((((((((";
    sizeMarker = sizeMarker * (float)percentage / (float)100;

    for ( int i = 0 ; i < videosVinculados->size(); i++ )
    {
        if ( videosVinculados->at( i )->name == videoName )
        {
            videosVinculados->at( i )->player->play();
            videosVinculados->at( i )->player->setVolume( volume );

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, videosVinculados->at( i )->grabber->textureId );
            glColor3f( 1, 1, 1 );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f,-1.0f);
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sizeMarker/2*( 16 / ( float )9 ), -sizeMarker/2, 0 );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sizeMarker/2*( 16 / ( float )9 ),  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sizeMarker/2*( 16 / ( float )9 ),  sizeMarker/2, 0 );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sizeMarker/2*( 16 / ( float )9 ), -sizeMarker/2, 0 );

            glEnd();
            glDisable( GL_TEXTURE_2D);
        }
    }
}

void Scene::decreaseVideosVolume()
{
    for ( int i = 0 ; i < videos->size(); i++ )

        if ( videos->at( i )->player->state() == QMediaPlayer::PlayingState )  {
//            emit message( "<div style=\"color:red;\">Marcador no detectado, el video se pausará</div>" );
            videos->at( i )->player->setVolume( videos->at( i )->player->volume() - 1 );

            if( videos->at( i )->player->volume() <= 0 )
            {
//                emit message( "Video pausado" );
                videos->at( i )->player->pause();
            }
        }
}

void Scene::decreaseVideosVolumeVinculados()
{
    for ( int i = 0 ; i < videosVinculados->size(); i++ )

        if ( videosVinculados->at( i )->player->state() == QMediaPlayer::PlayingState )  {
//            emit message( "<div style=\"color:red;\">Marcador no detectado, el video se pausará</div>" );
            videosVinculados->at( i )->player->setVolume( videosVinculados->at( i )->player->volume() - 1 );

            if( videosVinculados->at( i )->player->volume() <= 0 )
            {
//                emit message( "Video pausado" );
                videosVinculados->at( i )->player->pause();
            }
        }
}

void Scene::slot_vincular( int marker_id, QString recurso, QString formatoCaja )
{
    int ok = 0;
    if ( QFileInfo(recurso).suffix() == "mp4" )
        ok = Database::getInstance()->saveVinculo( marker_id, recurso, "n" );  // No se permite video en formato caja
    else
        ok = Database::getInstance()->saveVinculo( marker_id, recurso, formatoCaja );

    if ( ! ok )
        QMessageBox::information(this, "No se pudo vincular", "Intente de nuevo");

}

void Scene::slot_cambiarCamara(int nCamera)
{
    videoCapture->release();
    videoCapture->open(nCamera);
}

void Scene::slot_updateScene()  {

#ifndef OPENGL_ES

    videoCapture->operator >>( textures->operator []( 0 )->mat );

    // invierto la imagen para que se pueda poner la camara de forma horizontal
    rotate(textures->operator []( 0 )->mat, textures->operator []( 0 )->mat, 180);

    // roto la imagen para tener un efecto espejo
    cv::flip(textures->operator []( 0 )->mat, textures->operator []( 0 )->mat, 1);

    process( textures->operator []( 0 )->mat );

    textures->operator []( 0 )->generateFromMat();
    this->updateGL();
#else
    if ( ! videoCapture->isOpened() )
           return;

       videoCapture->operator >>( matCamera );

       this->process( matCamera );

       update();
#endif
}

void Scene::process( Mat &frame )
{
    std::vector< Marker > detectedMarkersVector;
#ifndef OPENGL_ES
    Mat grayscaleMat; cvtColor( frame, grayscaleMat, CV_BGR2GRAY );
    Mat binaryMat; threshold( grayscaleMat, binaryMat, 128, 255, cv::THRESH_BINARY );
    cameraParameters->resize( binaryMat.size() );
    markerDetector->detect( binaryMat, detectedMarkersVector, *cameraParameters, 0.08f );
    detectedMarkers = QVector< Marker >::fromStdVector( detectedMarkersVector );
#else
    cameraParameters->resize( frame.size() );
    markerDetector->detect( frame, detectedMarkersVector, *cameraParameters, 0.08f );
    detectedMarkers = QVector< Marker >::fromStdVector( detectedMarkersVector );
#endif


    // descripcion del marker
//       for( int i = 0; i < detectedMarkers.size(); i++ )
//           detectedMarkers.at( i ).draw( frame, Scalar( 255, 0, 255 ), 1 );



    QVector<Jugador*> * vp = Jugador::getJugadoresActuales();

    clearJugadores(vp);
    initJugadores(vp, detectedMarkers);

//    int winnerLine = 150;
//    int zonaTriangulacion = 75;
    drawGameLines(frame, winnerLine, zonaTriangulacion);

    // obtengo los markers a utilizar
    QVector<Marker> mvec;
    for( int i = 0; i < detectedMarkers.size(); i++ ) {
        Marker m = detectedMarkers.at( i );
        int mkr_y = m.getCenter().y;
        if( !(mkr_y < winnerLine) ) {
            mvec.append(detectedMarkers.at(i));
        }
    }


    // seteo la distancia a la winnerLine a cada marker de cada jugador
    setDistancesToWinnerLine(vp, winnerLine);
    // primero los que estan afuera de TODO. los elimino
    killOutOfZoneMarkers(vp, zonaTriangulacion);
    // luego determino quien puede triangular y dibujo la triangulacion
    determineWhoCanTriangulate(vp, winnerLine, zonaTriangulacion);
    drawPosibleTriangulation(frame, vp, 3);
    // finalmente, veo quienes quedaron encerrados en una triangulacion y los mato
    determineDeadPlayers(vp, winnerLine, zonaTriangulacion);
}

/**
 * @brief Scene::rotate rota el Mat src y lo deja en dst el angulo angle.
 */
void Scene::rotate(cv::Mat& src, cv::Mat& dst,  double angle){
    cv::Point2f ptCp(src.cols*0.5, src.rows*0.5);
    cv::Mat M = cv::getRotationMatrix2D(ptCp, angle, 1.0);
    cv::warpAffine(src, dst, M, src.size(), cv::INTER_CUBIC); //Nearest is too rough,
}

/**
 * @brief Scene::setDistancesToWinnerLine seteo todas las distancias a la winnerLine
 * de cada marker.
 * @param vp vector de jugadores a analizar.
 * @param winnerLine linea objetivo del juego.
 */
void Scene::setDistancesToWinnerLine(QVector<Jugador *> *vp, int winnerLine)
{
    Point targetY( 0, winnerLine ), nearest;
    float mdist = -1;
    vector<Point> cp;
    for(int i = 0; i < vp->size(); i++) {
        Jugador * j = vp->at(i);
        for(int k = 0; k < j->getFichas()->size(); k++) {
            QRCode * qrc = j->getFichas()->at(k);
            Point p( qrc->getCordX(), qrc->getCordY() );
            cp.push_back(p);
            calcShortestDistance( cp, targetY, nearest, mdist, DISTANCE::AXIS_Y );
            qrc->setDistanceToTarget(mdist);
            cp.clear();
        }
    }
}

/**
 * @brief Scene::killOutOfZoneMarkers pongo como no visibles, las fichas
 * que su centro esta fuera de zone.
 * @param vp vector de jugadores a analizar
 * @param zone valor maximo de Y en el area a buscar
 */
void Scene::killOutOfZoneMarkers(QVector<Jugador *> *vp, int zone)
{
    for(int i = 0; i < vp->size(); i++){
        Jugador * j = vp->at(i);

        for(int k = 0; k < j->getFichas()->size(); k++){
            QRCode * qrc = j->getFichas()->at(k);
            if( qrc->getCordY() >= zone ) {
                qrc->setVisible(false);
                qrc->setEstado(QRCode::ESTADO::dead);
            }
        }
    }
}

/**
 * @brief Scene::determineDeadPlayers recorro todos los jugadores
 * y las fichas de cada uno, y si estan visibles, veo si estan
 * encerradas por un poligono de algun jugador.
 * @param vp vector de jugadores a analizar
 */
void Scene::determineDeadPlayers(QVector<Jugador *> *vp, int winnerLine, int zonaTriangulacion)
{
    for(int i = 0; i < vp->size(); i++){
        Jugador * ji = vp->at(i);

        for(int k = 0; k < ji->getFichas()->size(); k++){
            QRCode * qrc = ji->getFichas()->at(k);


            if( qrc->getEstado() == QRCode::ESTADO::dead )
                continue;

            for(int n = 0; n < vp->size(); n++){
                Jugador * jj = vp->at(n);


                // tengo que matar un marker solo si algun marker
                // esta en la zona de triangulacion
                // solo le paso un jugador si tiene un marker en zona de triangulacion
                bool isInTriangulationZone = isInZone(jj, zonaTriangulacion, winnerLine);

                // dos jugadores son el mismo si estan en la misma posicion del vector vp
                if ( i != n && isInTriangulationZone )
                    markerInPolygon(qrc, jj);
            }
        }
    }
}

/**
 * @brief Scene::markerInPolygon determina si qrc esta encerrado
 * en una triangulacion hecha por el jugador jug.
 * @param qrc es un QRCode, de su posicion se determina si esta
 * dentro del poligono de otro vector.
 * @param jug es un jugador, del cual se analiza su vector de fichas
 * y se guardan las posiciones de estas para formar el poligono.
 * @return true en caso de que ese QRCode este dentro de alguna
 * triangulacion valida por las fichas del jugador jug
 */
bool Scene::markerInPolygon(QRCode * qrc, Jugador * jug)
{
    QVector<float> *vertx = new QVector<float>();
    QVector<float> *verty = new QVector<float>();
    int nvert = 0;

    for(int i = 0; i < jug->getFichas()->size(); i++){
        QRCode * code = jug->getFichas()->at(i);

        if( code->getEstado() == QRCode::ESTADO::valid || code->getEstado() == QRCode::ESTADO::triangulation ){
            vertx->append(code->getCordX());
            verty->append(code->getCordY());
            nvert++;
        }
    }

    int res = pnpoly(nvert, vertx, verty, qrc->getCordX(), qrc->getCordY());

    if(res) {
//        qDebug() << "marker:" << qrc->getMkr().id << " DESACTIVADO";
        qrc->setVisible(false);
        qrc->setEstado(QRCode::ESTADO::dead);
        return false;
    }
}

/**
 * @brief Scene::drawGameLines dibujo las lineas de cada zona del juego
 */
void Scene::drawGameLines(Mat &frame, int winnerLine, int zonaTriangulacion)
{
    int height = frame.rows;
    int refLine = height - winnerLine;
    int zona1 = winnerLine + (refLine / 2);

    // winner line
    cv::line(frame, Point(0, winnerLine), Point(640, winnerLine), Scalar(0, 255, 0), 3);
    // zona de triangulacion
    cv::line(frame, Point(0,zonaTriangulacion), Point(640, zonaTriangulacion), Scalar(255,0,0), 2);
    // zona 1000 puntos
//    cv::line(frame, Point(0,zona1), Point(640, zona1), Scalar(0,255,0), 2);
}

/**
 * @brief Scene::isInZone determina si el jugador j tiene un QRCode
 * que esta en la zona delimitada. retorna true si almenos uno se encuentra.
 * @param j jugador del cual se recorre su vector de fichas
 * @param maxY, minY delimitan la zona.
 * @return true si al menos QRCode esta en la zona, sino false.
 */
bool Scene::isInZone(Jugador * j, int maxPos, int minPos)
{
    for (int r = 0; r < j->getFichas()->size(); r++){
        QRCode * q = j->getFichas()->at(r);
        if ( q->getCordY() < maxPos && q->getCordY() > minPos )
            return true;
    }
    return false;
}

/**
 * @brief Scene::determineWhoCanTriangulate determina y setea en estado triangulation.
 * Pone a los markers que estan, en la zona de triangulacion (entre zonaTriangulacion y winnerLine).
 * En caso de que varios markers esten en esa zona (aunque sean de diferentes equipos)
 * elige el que esta mas proximo a la winnerLine
 * @param vp vector de jugadores a analizar
 * @param winnerLine linea a la que se debe estar mas proximo sin pasarse
 * @param zonaTriangulacion altura que delimita cuando podes triangular,
 */
void Scene::determineWhoCanTriangulate(QVector<Jugador *> *vp, int winnerLine, int zonaTriangulacion)
{
    QRCode * q = NULL;
    float mdist = 480;
    float lastMdist = 480;
    for ( int i = 0 ; i < vp->size(); i++ ){
        Jugador * j = vp->at(i);
        vector<Point> cp;
        cp.clear();
        Point target(0, winnerLine);
        Point nearestp;
        bool exists = false;

        for( int k = 0; k < j->getFichas()->size(); k++ ) {
            QRCode * qrc = j->getFichas()->at(k);
            Point p( qrc->getCordX(), qrc->getCordY() );

            if ( qrc->getCordY() <= zonaTriangulacion && qrc->getCordY() >= winnerLine ) {
                qrc->setEstado(QRCode::ESTADO::dead);
                cp.push_back(p);
                exists = true;
            }

            if (cp.size() == 0) continue;
            calcShortestDistance(cp, target, nearestp, mdist, DISTANCE::AXIS_Y);

            if ( mdist < lastMdist && exists) {
//                qDebug() << "jug:"<< i << "marker:" << qrc->getMkr().id << "distancia:" << mdist;
                exists = false;
                lastMdist = mdist;
                q = qrc;
            }
        }
    }

    if(q != NULL ){
//        qDebug() << "marker triangulacion valida:" << q->getMkr().id;
        q->setEstado(QRCode::ESTADO::triangulation);
    }
}

/**
 * @brief Scene::initJugadores inicializo los jugadores que vienen
 * como parametro y verifico si en el vector de marker detectados
 * que viene como parametro esta algun marker que pertenece a algun
 * jugador. En ese caso, crea un QRcode y lo agrega al vector
 * de fichas de cada jugador.
 * @param vp vector de jugadores que quiero inicializar
 * @param dm vector de markers que quiero setear a cada jugador
 */
void Scene::initJugadores(QVector<Jugador *> *vp, QVector<Marker> &dm)
{
    // seteo el centro de marker detectado y la cantidad a cada jugador
    for(int i = 0; i < vp->size(); i++){

        Jugador * j = vp->at(i);

        for( int k = 0; k < dm.size(); k++ ) {

            Marker m = dm.at( k );
            Point mkr_center = m.getCenter();

            if(j->getVecids()->indexOf(m.id) != -1) {
                QPoint p(mkr_center.x, mkr_center.y);
                QRCode * qrc = QRCode::createQRC( m, p.x(), p.y(), true, QRCode::ESTADO::valid );

                j->getFichas()->append(qrc);
            }
        }
    }
}

/**
 * @brief Scene::clearJugadores borro las fichas de cada jugador,
 * porque estas se actualizan en cada frame.
 * @param vp vector de jugadores que quiero reestablecer.
 */
void Scene::clearJugadores(QVector<Jugador *> *vp)
{
    for(int i = 0; i < vp->size(); i++) {
        Jugador * j = vp->at(i);
        j->getFichas()->clear();
    }
}

/*
nvert: Number of vertices in the polygon. Whether to repeat the first
vertex at the end has been discussed in the article referred above.
==> cantidad de markers detectados para ese jugador

vertx, verty: Arrays containing the x- and y-coordinates of the polygon's vertices.
==> por cada marker detectado de cada jugador, agrego la posicion en x e y a un vector

testx, testy: X- and y-coordinate of the test point.
==> coordenadas de cada marker, para ver cual esta dentro de un
poligono
 */
int Scene::pnpoly(int nvert, QVector<float> *vertx, QVector<float> *verty, float testx, float testy)
{
    int i, j, c = 0;
     for (i = 0, j = nvert-1; i < nvert; j = i++) {
       if ( ( (verty->at(i) > testy) != (verty->at(j) > testy) ) &&
        (testx < (vertx->at(j) - vertx->at(i) ) * (testy - verty->at(i) ) / (verty->at(j) - verty->at(i) ) + vertx->at(i) ) )
          c = !c;
     }
     return c;
}

/**
 * @brief Scene::drawLinesBeetweenMarkers Por cada jugador, itera sobre los markes
 * visibles de cada jugador y desde el centro de cada marker dibuja una linea.
 * @param frame imagen donde va a dibujar
 * @param thickness grosor de la linea. 3 por defecto
 */
void Scene::drawLinesBeetweenMarkers(Mat &frame, QVector<Jugador *> * vp, int thickness)
{
    vector<Point> cp;
    for(int i = 0; i < vp->size(); i++){
        Jugador * j = vp->at(i);
        cp.clear();

        for( int k = 0; k < j->getFichas()->size(); k++ ) {

            QRCode * qrc = j->getFichas()->at( k );
            Marker m = qrc->getMkr();
            Marker * m1 = &m;
            Point mkr_center = m1->getCenter();

//            qDebug() << "drawLinesBeetweenMarkers visible:" << qrc->getVisible();
            if(j->getVecids()->indexOf(m.id) != -1 && ( qrc->getEstado() != QRCode::ESTADO::dead) ){
                cp.push_back(Point(mkr_center.x , mkr_center.y));
            }
        }

        if(cp.size() != 0){
            QColor c = j->getTeamColor();
            Scalar color(c.red(), c.green(), c.blue());
            CvDrawing::LinesBetweenPoints(frame, cp, color, thickness);
        }
    }
}

/**
 * @brief Scene::drawPosibleTriangulation una vez determinadas las triangulaciones
 * (metodo determineWhoCanTriangulate) se llama a este metodo para que dibuje
 * lineas entre los markers del mismo equipo, si es que alguno esta en estado
 * triangulation.
 * @param frame imagen donde va a dibujar
 * @param vp vector de jugadores a analizar
 * @param thickness grosor de la linea a dibujar.
 */
void Scene::drawPosibleTriangulation(Mat &frame, QVector<Jugador *> * vp, int thickness)
{
    QVector<Jugador*> * triangulationplayers = new QVector<Jugador*>();
    for(int i = 0; i < vp->size(); i++){
        Jugador * jug = vp->at(i);
        for(int j = 0; j < jug->getFichas()->size(); j++ ){
            QRCode * qrc = jug->getFichas()->at(j);
            if( qrc->getEstado() == QRCode::ESTADO::triangulation ) {
                triangulationplayers->append(jug);
            }
        }
    }

    if( triangulationplayers->size() > 0){
        //* dibujo lienas entre markers del mismo equipo
        drawLinesBeetweenMarkers(frame, triangulationplayers, thickness);
    }
}

/**
 * @brief Scene::drawShortestDistance dibuja la linea mas corta al punto target
 * de cada equipo, respetando el type de medicion: MODULO, AXIS_Y o AXIS_X
 * @param frame el mat donde dibuja
 * @param mkrs los markers que se quieren analizar, podrian no ser todos por
 * eso pongo el parametro.
 * @param target el punto de referencia donde va a calcular las lineas
 * @param type tipo de medicion, respecto a eje x, y o ambos siendo
 * AXIS_X, AXIS_Y o MODULE los valores respectivamente.
 * @param thickness el grosor de las lineas. 3 por default
 */
void Scene::drawShortestDistance(Mat &frame, QVector<Jugador *> * vp ,QVector< Marker > mkrs,  Point target, DISTANCE type, int thickness)
{
    vector<Point> cp;
    QVector<QString> NearestMkrs;
    for(int i = 0; i < vp->size(); i++){
        Jugador * j = vp->at(i);
        cp.clear();

        Point npt;
        float minDist = -1;

        for( int k = 0; k < mkrs.size(); k++ ) {
            Marker m = mkrs.at( k );
            Point mkr_center = m.getCenter();

            // obtengo un vector con los puntos en X de los markers
            if(j->getVecids()->indexOf(m.id) != -1){

                // preguntar si el id que es valido para ese jugador
                // corresponde a un QRCode que esta visible en el juego
                for(int n = 0; n < j->getFichas()->size(); n++){
                    QRCode * qrc = j->getFichas()->at(n);
                    if ( qrc->getMkr().id == m.id && qrc->getEstado() == QRCode::ESTADO::valid )
                        cp.push_back(mkr_center);
                }
            }
        }

        // calculo la minima distancia respecto eje Y
        calcShortestDistance(cp, target, npt, minDist, DISTANCE::AXIS_Y);

        if(minDist != -1 || cp.size() != 0){
            QColor c = j->getWin_color();
            Scalar co(c.red(), c.green(), c.blue());

            switch (type) {
            case AXIS_Y:
                cv::line(frame, Point(npt.x, target.y), npt, co, thickness);
                break;

            case AXIS_X:
                cv::line(frame, Point(target.x, npt.y), npt, co, thickness);
                break;

            default:
                cv::line(frame, target, npt, co, thickness);
                break;
            }

            int md = minDist;
            NearestMkrs.append(j->getNombre() + " esta a " + QString::number(md));
        }
    }

    for(int i = 0; i < NearestMkrs.size(); i++) {
        cv::putText(frame, NearestMkrs.at(i).toStdString(), Point(0, 30 + i * 30) , FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0));
    }
}

/**
 * @brief Scene::calcShortestDistance calcula la distancia mas corta entre los puntos
 * del vector cp y el target. Carga el punto mas cercano en nearestp y la distancia
 * en mdist.
 * @param cp vector de puntos
 * @param target punto de referencia para calcular las distancias
 * @param nearestp objeto donde carga el punto mas cercano
 * @param mdist distancia minima entre los puntos del vector y el target
 * @param type tipo de medicion, respecto a eje x, y o ambos siendo
 * AXIS_X, AXIS_Y o MODULE los valores respectivamente.
 */
void Scene::calcShortestDistance(vector<Point> &cp, Point target, Point &nearestp, float &mdist, DISTANCE type)
{
    int sz = cp.size();
    if(sz == 0) return;

    vector<Point> copy = cp;

    if(type == AXIS_Y){
        vector<Point> cpy;
        for(int i = 0; i < sz; i++){
            Point p = copy.at(i);
            cpy.push_back(Point(0,p.y));
        }
        copy = cpy;
    }

    if(type == AXIS_X){
        vector<Point> cpx;
        for(int i = 0; i < sz; i++){
            Point p = copy.at(i);
            cpx.push_back(Point(p.x, 0));
        }
        copy = cpx;
    }

    nearestp = copy.at(0);
    mdist = sqrt( ( (target.x - nearestp.x) * (target.x - nearestp.x) )  +  ( (target.y - nearestp.y) * (target.y - nearestp.y) ) );
    nearestp = cp.at(0);
    for(int i = 0; i < copy.size(); i++){
        Point n = copy.at(i);
        float dist = sqrt( ( (target.x - n.x) * (target.x - n.x) )   +  ( (target.y - n.y) * (target.y - n.y) ) );
        if(dist < mdist) {
            mdist = dist;
            nearestp = cp.at(i);
        }
    }
}


#ifdef OPENGL_ES

void Scene::crearProgram()
{
    program = new QOpenGLShaderProgram;

    // Compile vertex shader
    if ( ! program->addShaderFromSourceFile( QOpenGLShader::Vertex, ":/vshader.glsl" ) )
        QMessageBox::critical( this, "No se cargo vshader.glsl", "Falla" );

    // Compile fragment shader
    if ( ! program->addShaderFromSourceFile( QOpenGLShader::Fragment, ":/fshader.glsl" ) )
        QMessageBox::critical( this, "No se cargo fshader.glsl", "Falla" );

    // Link shader pipeline
    if ( ! program->link() )
        QMessageBox::critical( this, "No se Linkeo", "Falla" );

    // Bind shader pipeline for use
    if ( ! program->bind() )
        QMessageBox::critical( this, "No se bindeo", "Falla" );

}


/*
 * genera las coordenadas (vertices e indices) donde se pegara la imagen de la camara y
 * los almacena en 'cam_buffer' para que se levante la imagen de la camara
*/
void Scene::crearCam()
{
    cam_buffer = new QOpenGLBuffer( QOpenGLBuffer::VertexBuffer );
    cam_buffer->create();

    static const int coords[1][4][3] = { { { +1, -1, -999 }, { -1, -1, -999 }, { -1, +1, -999 }, { +1, +1, -999 } } };

    QVector<GLfloat> vertData;
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append(coords[ i ][ j ][ 0 ]);
            vertData.append(coords[ i ][ j ][ 1 ]);
            vertData.append(coords[ i ][ j ][ 2 ]);

            // texture coordinate
            if ( j==0 )  {  vertData.append( 1 );  vertData.append( 0 );  }
            if ( j==1 )  {  vertData.append( 0 );  vertData.append( 0 );  }
            if ( j==2 )  {  vertData.append( 0 );  vertData.append( 1 );  }
            if ( j==3 )  {  vertData.append( 1 );  vertData.append( 1 );  }
        }
    }

    if ( ! cam_buffer->bind() )
        QMessageBox::critical(this, "False", "vbo2 bind()");

    cam_buffer->allocate( vertData.constData(), vertData.count() * sizeof( GLfloat ) );

    cam_texture = new QOpenGLTexture( QOpenGLTexture::Target2D );
    cam_texture->setMinificationFilter( QOpenGLTexture::Nearest );
    cam_texture->setMagnificationFilter( QOpenGLTexture::Linear );
    cam_texture->setFormat( QOpenGLTexture::RGBA8_UNorm );

    // Estos valores hay que sacarlo directamente de los Mat que entrega el OpenCV
    cam_texture->setSize( 640, 480 );
}

/*
 * 'cam_buffer' contiene las coodenadas (vertices y indices) donde se dibujara
 * la imagen de la camara, es por esto que hacemos bind() para utilizar y indicar que vamos
 * a dibujar sobre ese plano
 *
 * 'projection' sera nuestra matriz de projeccion que definimos como orthonormal
*/
void Scene::drawCamera()
{
    cam_buffer->bind();

    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.ortho( -1.0f, +1.0f, +1.0f, -1.0f, -10.0f, 1000.0f );

    program->setUniformValue( "projection", projection );
    program->enableAttributeArray( PROGRAM_VERTEX_ATTRIBUTE );
    program->enableAttributeArray( PROGRAM_TEXCOORD_ATTRIBUTE );
    program->setAttributeBuffer( PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof( GLfloat ) );
    program->setAttributeBuffer( PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof( GLfloat ), 2, 5 * sizeof( GLfloat ) );

    if ( matCamera.rows > 0 )  {
        cv::Mat temp( matCamera.cols, matCamera.rows, matCamera.type() );
        cvtColor( matCamera, temp, CV_BGR2RGB );

//        cam_texture->setSize( matCamera.cols, matCamera.rows );

        if ( ! cam_texture->isStorageAllocated() )  {
            cam_texture->allocateStorage();
        }

        cam_texture->setData( QOpenGLTexture::RGB, QOpenGLTexture::UInt8, temp.data );

        cam_texture->bind();

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

#endif



#include "scene.h"
#include <QApplication>
#include "ui_principal.h"  // Necesario para acceder al ui de Principal



/* Junior del futuro:
 *
 * TODO:
 * 1- averiguar como hace para que la imagen
 * "siga" el marcador, ya se como setear la imagen,
 * pero quiero saber como hace eso antes de continuar
 * con el proyecto.
 *
 * Status: terminado.
 * Lo hace en paintgl donde a cada marker le pide su matrix y luego la setea
 *
 * TODO:
 * 2- para saber si un marker esta relacionado, pregunto
 * en la bd si existe la relacion (num_jug, marker_id)
 * pero no esta bueno que sea asi, deberia ser mas eficiente.
 *
 * Status: terminado
 *
 * 3- Determinar en que momento puedo dibujar en pantalla
 *    en el paintgl no puedo => y tampoco deberia si lo hago con opencv.
 * pero si dibujo en process, las lineas se ven abajo del marker
 *
 */

Scene::Scene( QWidget *parent ) : QGLWidget( parent ),
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

                                  principal ( (Principal*) parent )
{

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

void Scene::loadTextures()
{
#ifdef PORTABLE
    QDir directory( "./Textures" );
#else
    QDir directory( APPLICATION_PATH "Textures" );
#endif

    QStringList fileFilter;
    fileFilter << "*.jpg" << "*.png" << "*.bmp" << "*.gif";
    QStringList imageFiles = directory.entryList( fileFilter );

    for ( int i = 0; i < imageFiles.size(); i++ )
    {
        textures->append( new Texture( imageFiles.at( i ) ) );
        QString textureUri = APPLICATION_PATH "Textures/" + imageFiles.at( i );

        Mat textureMat = imread( textureUri.toStdString() );
        flip( textureMat, textureMat, 0 );
        textures->last()->mat = textureMat;
        textures->last()->generateFromMat();
    }
}

void Scene::addTexture(QString imagen){

    textures->append(new Texture(imagen));
    Mat textureMat = imread( imagen.toStdString() );
    flip( textureMat, textureMat, 0 );
    textures->last()->mat = textureMat;
    textures->last()->generateFromMat();
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
    initializeGLFunctions();

    glClearColor( 0, 0, 0, 0 );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );

    GLfloat lightAmbient[4]; lightAmbient[0] = 0.5f;  lightAmbient[1] = 0.5f;
            lightAmbient[2] = 0.5f;  lightAmbient[3] = 1.0f;

    GLfloat lightDiffuse[4]; lightDiffuse[0] = 1.0f;  lightDiffuse[1] = 1.0f;
            lightDiffuse[2] = 1.0f;  lightDiffuse[3] = 1.0f;

    GLfloat lightPosition[4];lightPosition[0]= 0.0f;  lightPosition[1]= 0.0f;
            lightPosition[2]= 2.0f;  lightPosition[3]= 1.0f;

    glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );  glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse );
    glLightfv( GL_LIGHT1, GL_POSITION,lightPosition ); glEnable( GL_LIGHT1 );

    textures->append( new Texture( "CameraTexture" ) );

    loadTextures();
    emit message( "Texturas cargadas" );

    actualizarTexturas();
    emit message( "Texturas vinculadas cargadas" );

    loadModels();
    emit message( "Modelos cargados" );

    loadVideos();
    emit message( "Videos cargados" );
}

void Scene::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );
}

void Scene::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
    float coefTamano = (float)principal->ui->sbTamano->value() / (float)100;
    QVector<Jugador*> * vp = Jugador::getJugadoresActuales();
    for(int i = 0; i < vp->size(); i++){

        Jugador * jug = vp->at(i);
        QString fp = jug->getFoto_perfil();

        for( int j = 0 ; j < detectedMarkers.size() ; j++ ){

            // aca se toma la matrix de cada marcador
            detectedMarkers.operator []( j ).glGetModelViewMatrix( modelview_matrix );
            // y se setea la matrix de ese marcador, para que la pueda dibujar y seguir
            glLoadMatrixd( modelview_matrix );


            //-------------------Inicio: dibujo imagen en marker ----------------------
            int mi = detectedMarkers.at(j).id;            
            // pregunto si el jugador tiene en su vector de ids, el
            // id del marcador encontrado y dibujo su foto si esta esa relacion.
            if( jug->getVecids()->indexOf(mi) != -1) {
                drawSheet(fp, detectedMarkers.at( j ).ssize, 100 * coefTamano);
            }
            //-------------------Fin: dibujo imagen en marker ----------------------

            // si detecto el marker especial
            if( mi == 108 ){
                drawSheet("/home/jrjs/obj.png", detectedMarkers.at( j ).ssize, 100 * coefTamano);
            }

        }
    }

    // Fin: Gráfico en marcadores
    glFlush();

}

void Scene::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() )  {
    case Qt::Key_M:
        if ( principal->isFullScreen() )  {
            principal->showMaximized();
            principal->setVisibleSliders(true);
        }
        else  {
            principal->showFullScreen();;
            principal->setVisibleSliders(false);
        }
        break;

    case Qt::Key_Escape:
        qApp->quit();
        break;

    default:;
    }
}

void Scene::drawCamera( unsigned int percentage )
{
    qDebug() << "%%%%%%%%%%%%%%%%%%%% drawCamera %%%%%%%%%%%%%%%%%%%%%%%";
    drawSheet( "CameraTexture", percentage );
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
//    qDebug() << "<<<<<<<<<<<<<<<<< drawSheet <<<<<<<<<<<<<<<<<<<<<<<<<";
    sizeMarker = sizeMarker * (float)percentage / (float)100;
    for( int i = 0; i < textures->size(); i++ )
    {
        if( textures->at( i )->name == textureName )
        {
//            qDebug() << "------------drawSheet entra a dibujar------------";
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
}

/**
 * @brief Scene::drawBox Dibuja una caja. Aruco entrega el punto (0,0,0) justo en el centro del marcador.
 * @param textureName Nombre del archivo de imagen en la carpeta Textures. Sera la textura de la caja.
 * @param sizeMarker Tamano del marcador que lo entrega Aruco en el atributo ssize de la clase Marker.
 * @param percentage Para redimensionar la caja. En escala 1:1 la caja tiene el tamano del marcador.
 */
void Scene::drawBox( QString textureName, float sizeMarker, unsigned int percentage )
{
    qDebug() << """""""""""""""""drawBox""""""""""""""""""""""";
    sizeMarker = sizeMarker * (float)percentage / (float)100;

    for( int i = 0; i < textures->size(); i++ )
    {
        if( textures->at( i )->name == textureName )
        {
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

void Scene::slot_updateScene()  {

    videoCapture->operator >>( textures->operator []( 0 )->mat );
    process( textures->operator []( 0 )->mat );
    textures->operator []( 0 )->generateFromMat();
    this->updateGL();
}

void Scene::process( Mat &frame )
{
    Mat grayscaleMat; cvtColor( frame, grayscaleMat, CV_BGR2GRAY );
    Mat binaryMat; threshold( grayscaleMat, binaryMat, 128, 255, cv::THRESH_BINARY );

    std::vector< Marker > detectedMarkersVector;
    cameraParameters->resize( binaryMat.size() );
    markerDetector->detect( binaryMat, detectedMarkersVector, *cameraParameters, 0.08f );
    detectedMarkers = QVector< Marker >::fromStdVector( detectedMarkersVector );


    // descripcion del marker
    if ( principal->ui->cbMostrarId->isChecked() ) {
        for( int i = 0; i < detectedMarkers.size(); i++ ) {
            detectedMarkers.at( i ).draw( frame, Scalar( 255, 0, 255 ), 1 );
        }
    }

    // obtengo el target de mkr especial
    Point target;
    for( int i = 0; i < detectedMarkers.size(); i++ ) {
        if(detectedMarkers.at( i ).id == 108){
            target = detectedMarkers.at(i).getCenter();
        }
    }

    // dibujo de lineas por equipos
    Scalar c1(255,255,0), c2(255,0,255);
    vector<Scalar> vsc = {c1, c2};
    int thickness = 8;
//    drawLinesBeetweenMarkers(frame, vsc, thickness);


    // dibujo linea mas corta
    drawShortestLineToReference(frame, target);
}


/**
 * @brief Scene::drawLinesBeetweenMarkers Por cada jugador, itera sobre los markes
 * visibles de cada jugador y desde el centro de cada marker dibuja una linea.
 * @param frame imagen donde va a dibujar
 * @param vsc vector de colores, uno por jugador
 * @param thickness grosor de la linea. 3 por defecto
 */
void Scene::drawLinesBeetweenMarkers(Mat &frame, vector<Scalar> vsc, int thickness)
{
    vector<Point> cp;
    QVector<Jugador*> * vp = Jugador::getJugadoresActuales();
    if(vp->size() != vsc.size() ) {
        qDebug() << "ERROR: void Scene::drawLinesBeetweenMarkers. no hay suficientes colores para cada jugador";
        return;
    }

    for(int i = 0; i < vp->size(); i++){
        Jugador * j = vp->at(i);
        cp.clear();
        for( int k = 0; k < detectedMarkers.size(); k++ ) {
            Marker m = detectedMarkers.at( k );
            Point mkr_center = m.getCenter();

            if(j->getVecids()->indexOf(m.id) != -1)
                cp.push_back(Point(mkr_center.x , mkr_center.y));
        }
        if(cp.size() != 0){
            QColor c = j->getTeamColor();
            Scalar color(c.red(), c.green(), c.blue());
            CvDrawing::LinesBetweenPoints(frame, cp, color, thickness);
        }
    }
}

/**
 * @brief Scene::drawShortestLineToReference dibuja la linea mas corta al target
 * de cada equipo
 * @param frame el mat donde dibuja
 * @param target el punto de referencia donde va a calcular las lineas
 * @param thickness el grosor de las lineas
 */
void Scene::drawShortestLineToReference(Mat &frame, Point target, int thickness)
{
    vector<Point> cp;
    QVector<float> mrkcercanos;
    QVector<QString> NearestMkrs;
    QVector<Jugador*> * vp = Jugador::getJugadoresActuales();
    for(int i = 0; i < vp->size(); i++){
        Jugador * j = vp->at(i);
        cp.clear();

        Point npt;
        float minDist = -1;

        for( int k = 0; k < detectedMarkers.size(); k++ ) {
            Marker m = detectedMarkers.at( k );
            Point mkr_center = m.getCenter();

            if(j->getVecids()->indexOf(m.id) != -1)
                cp.push_back(mkr_center);
        }

        calcShortestDistance(cp, target, npt, minDist);

        if(minDist != -1 || cp.size() != 0){
            QColor c = j->getWin_color();
            Scalar co(c.red(), c.green(), c.blue());
            cv::line(frame, target, npt, co, thickness);
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
 * en mdist
 * @param cp vector de puntos
 * @param target punto de referencia para calcular las distancias
 * @param nearestp objeto donde carga el punto mas cercano
 * @param mdist distancia minima entre los puntos del vector y el target
 */
void Scene::calcShortestDistance(vector<Point> &cp, Point target, Point &nearestp, float &mdist)
{
    int sz = cp.size();
    if(sz == 0) return;

    nearestp = cp.at(0);
    mdist = sqrt( ( (target.x - nearestp.x) * (target.x - nearestp.x) )  +  ( (target.y - nearestp.y) * (target.y - nearestp.y) ) );
    for(int i = 0; i < sz; i++){

        Point n = cp.at(i);
        float dist = sqrt( ( (target.x - n.x) * (target.x - n.x) )   +  ( (target.y - n.y) * (target.y - n.y) ) );
        if(dist < mdist) {
            mdist = dist;
            nearestp = n;
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




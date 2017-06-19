#ifndef SCENE_H
#define SCENE_H

#define RESOLUTION_WIDTH  640
#define RESOLUTION_HEIGHT 480

#include <stdio.h>
#include <stdlib.h>



#include <QDir>
#include <QFile>
#include <QTimer>
#include <QVector>
#include <QSlider>
#include <QGLWidget>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QGLFunctions>


#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <aruco/aruco.h>

#include "common.h"

#include "controller/model.h"
#include "video.h"
#include "controller/texture.h"
#include "model/database.hpp"
#include "view/principal.h"
#include "model/jugador.h"
#include "controller/cvdrawing.h"



using namespace cv;
using namespace std;
using namespace aruco;

class Scene : public QGLWidget, protected QGLFunctions  {
    Q_OBJECT

private:
    int nCamera;
    VideoCapture *videoCapture;

    QTimer *sceneTimer;
    int milisecondsTimer;

    QVector< Texture * > *textures, *texturesVinculadas;
    QVector< Model * > *models;
    QVector< Video * > *videos, *videosVinculados;

    MarkerDetector *markerDetector;
    QVector< Marker > detectedMarkers;
    CameraParameters *cameraParameters;

    int zRotationVelocity;

    QVector<QStringList> imageFiles;

    Principal * principal;

    void loadTextures();
    void loadModels();
    void prepareModels();
    void loadTexturesForModels();
    void loadVideos();

    void process( Mat &frame );

    void drawCamera(unsigned int percentage = 100 );
    void drawCameraBox( unsigned int percentage = 100 );
    void drawSheetVinculadas( QString textureName, float sizeMarker, unsigned int percentage = 100 );
    void drawSheet( QString textureName, float sizeMarker, unsigned int percentage = 100 );
    void drawBox( QString textureName, float sizeMarker, unsigned int percentage = 100 );
    void drawBoxVinculado( QString textureName, float sizeMarker, unsigned int percentage = 100 );
    void drawModel( QString modelName, int percentage = 100 );
    void drawVideo( QString videoName, float sizeMarker, unsigned int percentage = 100, int volume = 100 );
    void drawVideoVinculado( QString videoName, float sizeMarker, unsigned int percentage = 100, int volume = 100 );
    void decreaseVideosVolume();
    void decreaseVideosVolumeVinculados();

    //
    void drawLinesBeetweenMarkers(Mat &frame, vector<Scalar> vsc, int thickness = 3);
    void drawShortestLineToReference(Mat &frame, Point target, int thickness = 3);

    void calcShortestDistance(vector<Point> &cp, Point target, Point &nearestp, float &mdist);
    void distanceFromPoints();

public:
    Scene( QWidget *parent = 0 );
    ~Scene();
    void actualizarTexturas();

    QTimer *getSceneTimer() const;
    void setSceneTimer(QTimer *value);

    // jr
    void addTexture(QString imagen);

protected:
    void initializeGL();
    void resizeGL( int width, int height );
    void paintGL();

    void keyPressEvent( QKeyEvent *event );

private slots:
    void slot_updateScene();

public slots:
    void slot_cambiarCamara(int nCamera);
    void slot_vincular(int marker_id, QString recurso , QString formatoCaja);

signals:
    void message( QString text );
};

#endif // SCENE_H

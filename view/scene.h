#ifndef SCENE_H
#define SCENE_H

#define RESOLUTION_WIDTH  640
#define RESOLUTION_HEIGHT 480

#include <stdio.h>
#include <stdlib.h>



#include <QDir>
#include <QFile>
#include <QPoint>
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
#include "view/juego.h"
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

    Juego * juego;

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
    void drawLinesBeetweenMarkers(Mat &frame, QVector<Jugador *> *vp, int thickness = 3);
    void drawShortestLineToTarget(Mat &frame, Point target, int thickness = 3);
    void drawPosibleTriangulation(Mat &frame, QVector<Jugador *> *vp, int thickness);

    enum DISTANCE {MODULE, AXIS_X, AXIS_Y};
    void calcShortestDistance(vector<Point> &cp, Point target, Point &nearestp, float &mdist, DISTANCE type = MODULE);
    void drawShortestDistance(Mat &frame, QVector<Jugador *> *vp, QVector<Marker> mkrs, Point target, DISTANCE type = MODULE, int thickness = 3);
    int pnpoly(int nvert, QVector<float> *vertx, QVector<float> *verty, float testx, float testy);
    bool markerInPolygon(QRCode *qrc, Jugador *jug);
    void killOutOfZoneMarkers(QVector<Jugador *> *vp, int zone);
    void determineDeadPlayers(QVector<Jugador *> *vp, int winnerLine, int zonaTriangulacion);
    void drawGameLines(Mat &frame, int winnerLine, int zonaTriangulacion);
    bool isInZone(Jugador *j, int maxPos, int minPos);
    void determineWhoCanTriangulate(QVector<Jugador *> *vp, int winnerLine, int zonaTriangulacion);
    void setDistancesToWinnerLine(QVector<Jugador *> *vp, int winnerLine);

    void initJugadores(QVector<Jugador *> *vp, QVector<Marker> &dm);
    void clearJugadores(QVector<Jugador *> *vp);

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

#ifndef CVDRAWING_H
#define CVDRAWING_H

/**
 * @brief The CvDrawing class representa una clase de dibujo(lineas, circulos, etc).
 * usa opencv para dibujar.
 */

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


using namespace std;
using namespace cv;

class CvDrawing
{

public:
    CvDrawing();

    /**
     * @brief LinesBetweenPoints dibuja lineas entre todos los puntos que viene
     * por parametro.
     * @param frame imagen donde se va a dibujar
     * @param vec vector de puntos donde se dibuja
     * @param color color que va a tener las lineas
     * @param thickness grosor de las lineas
     */
    static void LinesBetweenPoints(Mat &frame, vector<Point> vec, Scalar color, int thickness = 3);
};

#endif // CVDRAWING_H

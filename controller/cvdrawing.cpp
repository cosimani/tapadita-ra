#include "cvdrawing.h"
#include <QDebug>

CvDrawing::CvDrawing()
{
}


void CvDrawing::LinesBetweenPoints(Mat &frame, vector<Point> vec, Scalar color , int thickness)
{
    int sz = vec.size();
    if(!sz) return;

    for(int i = 0; i < sz; i++) {
        if(i == (sz-1) )
            line( frame, vec.at(0), vec.at(sz-1), color, thickness);
        else
            line( frame, vec.at(i), vec.at(i+1), color, thickness);
    }
}

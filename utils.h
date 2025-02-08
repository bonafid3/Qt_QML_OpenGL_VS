#ifndef UTILS_H
#define UTILS_H

#include <qmath.h>
#include <QDebug>
#include <QVector3D>

#define qd qDebug()

#define toStr(x) QString::number(x,'T',3)

struct sPlane {
    sPlane(QVector3D _n, QVector3D _p): n(_n), p(_p){}
    QVector3D n; //plane normal
    QVector3D p; //point on plane
};

inline float d2r(float deg)
{
	return deg*M_PI/180.0f;
}

inline float r2d(float rad)
{
	return rad*180.0f/M_PI;
}

inline float clamp(float val, float min, float max)
{
    if (val > max)
        return max;
    if (val < min)
        return min;
    return val;
}

inline int random(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

inline float ssaoLerp(float a, float b, float f)
{
    return a + f * (b - a);
}

#endif // UTILS_H

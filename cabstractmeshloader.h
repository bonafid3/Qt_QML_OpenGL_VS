#ifndef CLOADERBASE_H
#define CLOADERBASE_H

#include <QVector3D>

#include "utils.h"

struct sTriangle {
    QVector3D v1, n1, t1;
    QVector3D v2, n2, t2;
    QVector3D v3, n3, t3;
};

class cAbstractMeshLoader
{
public:
    cAbstractMeshLoader();

    virtual void load(const QString &file) = 0;

    virtual QVector<sTriangle>& triangles();

    QVector<sTriangle> mTriangles;
};

#endif // CLOADERBASE_H

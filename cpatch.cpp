#include "cpatch.h"

#include <QMatrix4x4>

cPatch::cPatch(const int x, const int z, const int patchSize)
{
    QMatrix4x4 m;

    m.translate(QVector3D(x*patchSize, 0, z*patchSize));
    m.scale(QVector3D(patchSize, 0, patchSize));

    mData.push_back(m * QVector3D(0,0,0));
    mData.push_back(m * QVector3D(1,0,0));
    mData.push_back(m * QVector3D(1,0,1));
    mData.push_back(m * QVector3D(0,0,1));
}

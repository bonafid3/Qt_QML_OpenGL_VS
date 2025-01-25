#include "cabstractmeshloader.h"

cAbstractMeshLoader::cAbstractMeshLoader()
{
}

QVector<sTriangle> & cAbstractMeshLoader::triangles()
{
    return mTriangles;
}


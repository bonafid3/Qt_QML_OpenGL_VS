#ifndef CSTL_H
#define CSTL_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>

#include "cabstractmeshloader.h"

struct sHelper {
    QVector3D normal, v1, v2, v3;
};

class cStlLoader : public cAbstractMeshLoader
{
public:
    cStlLoader(cStlLoader *parent=0);

    void load(const QString &file);

    QByteArray mHeader;

    QMatrix4x4 getRM();
    QMatrix4x4 mModelMatrix;

    QString mFile;
private:
    cStlLoader *mParent=0;
};

#endif // CSTL_H

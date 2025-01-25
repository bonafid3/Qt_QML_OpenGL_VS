#ifndef CPATCH_H
#define CPATCH_H

#include <QVector>
#include <QVector3D>

class cPatch
{
public:
    cPatch(const int x, const int z, const int patchSize);

    QVector<QVector3D> mData;
};

#endif // CPATCH_H

#ifndef COBJLOADER_H
#define COBJLOADER_H

#include <QVector>
//#include <QtOpenGL>
#include <QMap>
#include "cabstractmeshloader.h"

struct sMaterial
{
    QString name;
    QVector3D Ka; // ambient
    QVector3D Kd; // diffuse
    QVector3D Ks; // specular
    QVector3D Ke; // specular
    float Ni; // specular exponent
    float Ns; // specular exponent
    float d; // dissolve like transparency
    QString map_Kd; // texture
    int illum;
};

struct sObject
{
    QString name;
    sMaterial material;
};

class cObjLoader : public cAbstractMeshLoader
{
public:

    cObjLoader();
    void load(const QString& file);

    QVector<sMaterial> mMaterials;

private:
    void loadMaterial(const QString& prefix, const QString& file);
};

#endif // COBJLOADER_H

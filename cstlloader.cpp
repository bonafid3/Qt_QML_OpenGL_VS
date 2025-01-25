#include "cstlloader.h"
#include <QFile>

cStlLoader::cStlLoader(cStlLoader *parent) : cAbstractMeshLoader(), mParent(parent)
{
}

void cStlLoader::load(const QString &file)
{
    QFile f(file);

    if(f.open(QFile::ReadOnly)) {
        QByteArray stl = f.readAll();

        mHeader = stl.mid(0, 80);

        qd << mHeader;

        quint32 triCnt = *(reinterpret_cast<quint32 *>(stl.mid(80, 4).data()));

        qd << "triangle count" << triCnt << sizeof(sTriangle);

        int offs = 84;

        mTriangles.reserve(triCnt);

        for(int i=0; i<triCnt; i++) {
            sHelper helper;
            sTriangle tri;

            memcpy(&helper, stl.mid(offs+i*50).constData(), sizeof(sHelper));

            tri.n1 = helper.normal;
            tri.n2 = helper.normal;
            tri.n3 = helper.normal;

            tri.v1 = helper.v1;
            tri.v2 = helper.v2;
            tri.v3 = helper.v3;

            tri.t1 = QVector3D(0.8, 0.2, 0);
            tri.t2 = QVector3D(0.8, 0.2, 0);
            tri.t3 = QVector3D(0.8, 0.2, 0);
            mTriangles.append(tri);
            qd << sizeof(sTriangle);
        }
    }
}

QMatrix4x4 cStlLoader::getRM()
{
    QMatrix4x4 result;
    if(mParent) {
        result = mParent->getRM();
    }
    return result * mModelMatrix;
}

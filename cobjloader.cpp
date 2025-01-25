#include "cobjloader.h"

#include <QFile>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

cObjLoader::cObjLoader() : cAbstractMeshLoader()
{
}

void cObjLoader::loadMaterial(const QString& prefix, const QString& file)
{
    QFile f(prefix + file);

    sMaterial mat;
    bool matProgress = false;
    if(f.open(QFile::ReadOnly)) {
        while (!f.atEnd()) {
            QString l(f.readLine(1024).trimmed());

            if(l.size() == 0) {
                if(matProgress) {
                    matProgress = false;
                    mMaterials.push_back(mat);
                }
                continue;
            }
            if(l.startsWith("#")) {
                continue;
            } else if(l.startsWith("newmtl ")) {
                matProgress = true;

                QRegExp r("newmtl\\s+([^ ]+)");
                if(r.indexIn(l, 0) != -1)
                {
                    mat.name = r.cap(1);
                }
            } else if(l.startsWith("Ka ")) { // ambient
                QRegExp r("Ka\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                if(r.indexIn(l, 0) != -1)
                {
                    mat.Ka = QVector3D(r.cap(1).toFloat(),r.cap(2).toFloat(),r.cap(3).toFloat());
                }
            } else if(l.startsWith("Kd ")) { // diffuse
                QRegExp r("Kd\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                if(r.indexIn(l, 0) != -1)
                {
                    mat.Kd = QVector3D(r.cap(1).toFloat(),r.cap(2).toFloat(),r.cap(3).toFloat());
                }
            } else if(l.startsWith("Ks ")) { // specular
                QRegExp r("Ks\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                if(r.indexIn(l, 0) != -1)
                {
                    mat.Ks = QVector3D(r.cap(1).toFloat(),r.cap(2).toFloat(),r.cap(3).toFloat());
                }
            } else if(l.startsWith("Ns ")) { // specular exponent
                QRegExp r("Ns\\s+([^ ]+)");
                if(r.indexIn(l, 0) != -1) {
                    mat.Ns = r.cap(1).toFloat();
                }
            } else if(l.startsWith("illum ")) { // illumination 0-7
                QRegExp r("illum\\s+([^ ]+)");
                if(r.indexIn(l, 0) != -1) {
                    mat.illum = r.cap(1).toInt();
                }
            } else if(l.startsWith("map_Kd ")) { // texture file
                QRegExp r("map_Kd\\s+([^ ]+)");
                if(r.indexIn(l, 0) != -1) {
                    mat.map_Kd = r.cap(1);
                }
            }
        }
        if(matProgress) {
            mMaterials.push_back(mat);
        }
    }
}

void cObjLoader::load(const QString & file)
{
    QVector<QVector3D> v, vn, vt;
    v.push_back(QVector3D(0, 0, 0));
    vn.push_back(QVector3D(0, 0, 0));
    vt.push_back(QVector3D(0, 0, 0));

    QFile f(file);
    const QString prefix = file.left(file.lastIndexOf("/") + 1);

    sMaterial currentMaterial;

    if (f.open(QFile::ReadOnly))
    {
        while (!f.atEnd())
        {
            QString l(f.readLine(1024).trimmed());

            if (l.size() == 0 || l.startsWith("#")) // comment
            {
                continue;
            }
            else if (l.startsWith("mtllib "))
            {
                QRegExp r("mtllib\\s+([^\n]+)");
                if (r.indexIn(l, 0) != -1)
                {
                    loadMaterial(prefix, r.cap(1));
                }
            }
            else if (l.startsWith("usemtl ")) // vertex
            {
                // v -3.4101800e-003 1.3031957e-001 2.1754370e-002
                QRegExp r("usemtl\\s+([^\n]+)");
                if (r.indexIn(l, 0) != -1)
                {
                    QString matName = r.cap(1);
                    foreach(sMaterial mat, mMaterials) {
                        if (mat.name == matName) {
                            currentMaterial = mat;
                        }
                    }
                }
            }
            else if (l.startsWith("v ")) // vertex
            {
                // v -3.4101800e-003 1.3031957e-001 2.1754370e-002
                QRegExp r("v\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                if (r.indexIn(l, 0) != -1)
                {
                    v.push_back(QVector3D(r.cap(1).toFloat(), r.cap(2).toFloat(), r.cap(3).toFloat()));
                }
            }
            else if (l.startsWith("vt ")) // texture coords
            {
                QRegExp r("vt\\s+([^ ]+)\\s+([^ ]+)");
                if (r.indexIn(l, 0) != -1)
                {
                    vt.push_back(QVector2D(r.cap(1).toFloat(), r.cap(2).toFloat()));
                }
            }
            else if (l.startsWith("vn ")) // vertex normal
            {
                QRegExp r("vn\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                if (r.indexIn(l, 0) != -1)
                {
                    vn.push_back(QVector3D(r.cap(1).toFloat(), r.cap(2).toFloat(), r.cap(3).toFloat()));
                }
            }
            else if (l.startsWith("f ")) // face
            {
                /* tri1 = 1,2,3 | tri2 = 1,3,4
                                4-----3
                                 |  /|
                                 | / |
                                 |/  |
                                1-----2
                */
                // QUAD
                // f 1/1/1 2/2/2 3/3/3 4/4/4
                QRegExp rq("^f\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)$");
                if (rq.indexIn(l, 0) != -1)
                {
                    QString v1 = rq.cap(1);
                    QString v2 = rq.cap(2);
                    QString v3 = rq.cap(3);
                    QString v4 = rq.cap(4);

                    // from quad we make 2 triangles
                    sTriangle tri1, tri2;

                    QRegExp r("^(\\d+)/?(\\d+)?/(\\d+)$");
                    if (r.indexIn(v1, 0) != -1)
                    {
                        int i1 = r.cap(1).toInt();
                        int i3 = r.cap(3).toInt();
                        bool ok;
                        int i2 = r.cap(2).toInt(&ok);
                        if (!ok) {
                            //qd << "capture failed1";
                            i2 = 0;
                            //i3 = 0;
                        }

                        tri1.v1 = v.at(i1);
                        tri1.n1 = vn.at(i3);
                        tri1.t1 = vt.at(i2);

                        tri2.v1 = v.at(i1);
                        tri2.n1 = vn.at(i3);
                        tri2.t1 = vt.at(i2);

                        if (currentMaterial.map_Kd.isEmpty()) {
                            tri1.t1 = tri1.t2 = tri1.t3 = currentMaterial.Kd;
                            tri2.t1 = tri2.t2 = tri2.t3 = currentMaterial.Kd;
                        }
                    }
                    if (r.indexIn(v2, 0) != -1)
                    {
                        int i1 = r.cap(1).toInt();
                        int i3 = r.cap(3).toInt();
                        bool ok;
                        int i2 = r.cap(2).toInt(&ok);
                        if (!ok) {
                            //qd << "capture failed2";
                            i2 = 0;
                            //i3 = 0;
                        }

                        tri1.v2 = v.at(i1);
                        tri1.n2 = vn.at(i3);
                        tri1.t2 = vt.at(i2);

                        if (currentMaterial.map_Kd.isEmpty()) {
                            tri1.t1 = tri1.t2 = tri1.t3 = currentMaterial.Kd;
                            tri2.t1 = tri2.t2 = tri2.t3 = currentMaterial.Kd;
                        }
                    }
                    if (r.indexIn(v3, 0) != -1)
                    {
                        int i1 = r.cap(1).toInt();
                        int i3 = r.cap(3).toInt();
                        bool ok;
                        int i2 = r.cap(2).toInt(&ok);
                        if (!ok) {
                            //qd << "capture failed3";
                            i2 = 0;
                            //i3 = 0;
                        }

                        tri1.v3 = v.at(i1);
                        tri1.n3 = vn.at(i3);
                        tri1.t3 = vt.at(i2);

                        tri2.v2 = v.at(i1);
                        tri2.n2 = vn.at(i3);
                        tri2.t2 = vt.at(i2);

                        if (currentMaterial.map_Kd.isEmpty()) {
                            tri1.t1 = tri1.t2 = tri1.t3 = currentMaterial.Kd;
                            tri2.t1 = tri2.t2 = tri2.t3 = currentMaterial.Kd;
                        }
                    }
                    if (r.indexIn(v4, 0) != -1)
                    {
                        int i1 = r.cap(1).toInt();
                        int i3 = r.cap(3).toInt();
                        bool ok;
                        int i2 = r.cap(2).toInt(&ok);
                        if (!ok) {
                            //qd << "capture failed4";
                            i2 = 0;
                            //i3 = 0;
                        }

                        tri2.v3 = v.at(i1);
                        tri2.n3 = vn.at(i3);
                        tri2.t3 = vt.at(i2);

                        if (currentMaterial.map_Kd.isEmpty()) {
                            tri1.t1 = tri1.t2 = tri1.t3 = currentMaterial.Kd;
                            tri2.t1 = tri2.t2 = tri2.t3 = currentMaterial.Kd;
                        }
                    }

                    tri1.n1 = tri1.n2 = tri1.n3 = QVector3D::crossProduct(tri1.v2 - tri1.v1, tri1.v3 - tri1.v1).normalized();
                    tri2.n1 = tri2.n2 = tri2.n3 = QVector3D::crossProduct(tri2.v2 - tri2.v1, tri2.v3 - tri2.v1).normalized();

                    mTriangles.append(tri1);
                    mTriangles.append(tri2);
                    continue;
                }

                // TRIANGLE
                // f 1/1/1 2/2/2 3/3/3
                QRegExp r("^f\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)$");
                if (r.indexIn(l, 0) != -1)
                {
                    QString v1 = r.cap(1);
                    QString v2 = r.cap(2);
                    QString v3 = r.cap(3);

                    sTriangle tri;

                    QRegExp r("^(\\d+)/?(\\d+)?/(\\d+)$");
                    if (r.indexIn(v1, 0) != -1)
                    {
                        int i1 = r.cap(1).toInt();
                        int i3 = r.cap(3).toInt();
                        bool ok;
                        int i2 = r.cap(2).toInt(&ok);
                        if (!ok) {
                            //qd << "capture failed5:" << l << i1 << i2 << i3;
                            i2 = 0;
                            //i3 = 0;
                        }

                        tri.v1 = v.at(i1);
                        tri.n1 = vn.at(i3);
                        tri.t1 = vt.at(i2);

                        if (currentMaterial.map_Kd.isEmpty()) {
                            tri.t1 = tri.t2 = tri.t3 = currentMaterial.Kd;
                        }
                    }

                    if (r.indexIn(v2, 0) != -1)
                    {
                        int i1 = r.cap(1).toInt();
                        int i3 = r.cap(3).toInt();
                        bool ok;
                        int i2 = r.cap(2).toInt(&ok);
                        if (!ok) {
                            //qd << "capture failed6";
                            i2 = 0;
                            //i3 = 0;
                        }

                        tri.v2 = v.at(i1);
                        tri.n2 = vn.at(i3);
                        tri.t2 = vt.at(i2);

                        if (currentMaterial.map_Kd.isEmpty()) {
                            tri.t1 = tri.t2 = tri.t3 = currentMaterial.Kd;
                        }
                    }

                    if (r.indexIn(v3, 0) != -1)
                    {
                        int i1 = r.cap(1).toInt();
                        int i3 = r.cap(3).toInt();
                        bool ok;
                        int i2 = r.cap(2).toInt(&ok);
                        if (!ok) {
                            //qd << "capture failed7";
                            i2 = 0;
                            //i3 = 0;
                        }

                        tri.v3 = v.at(i1);
                        tri.n3 = vn.at(i3);
                        tri.t3 = vt.at(i2);

                        if (currentMaterial.map_Kd.isEmpty()) {
                            tri.t1 = tri.t2 = tri.t3 = currentMaterial.Kd;
                        }
                    }

                    tri.n1 = tri.n2 = tri.n3 = QVector3D::crossProduct(tri.v2 - tri.v1, tri.v3 - tri.v1).normalized();
                    mTriangles.append(tri);
                }
            }
        }
    }
}

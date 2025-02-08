#include "cobjloader.h"

#include <QFile>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QRegularExpression>

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

                QRegularExpression r("newmtl\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if(match.hasMatch())
                {
                    mat.name = match.captured(1);
                }
            } else if(l.startsWith("Ka ")) { // ambient
                QRegularExpression r("Ka\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if(match.hasMatch())
                {
                    mat.Ka = QVector3D(match.captured(1).toFloat(), match.captured(2).toFloat(), match.captured(3).toFloat());
                }
            } else if(l.startsWith("Kd ")) { // diffuse
                QRegularExpression r("Kd\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);

                if (match.hasMatch())
                {
                    mat.Kd = QVector3D(match.captured(1).toFloat(), match.captured(2).toFloat(), match.captured(3).toFloat());
                }
            } else if(l.startsWith("Ks ")) { // specular
                QRegularExpression r("Ks\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if (match.hasMatch()) {
                    mat.Ks = QVector3D(match.captured(1).toFloat(), match.captured(2).toFloat(), match.captured(3).toFloat());
                }
            } else if(l.startsWith("Ns ")) { // specular exponent
                QRegularExpression r("Ns\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if (match.hasMatch()) {
                    mat.Ns = match.captured(1).toFloat();
                }
            } else if(l.startsWith("illum ")) { // illumination 0-7
                QRegularExpression r("illum\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if (match.hasMatch()) {
                    mat.illum = match.captured(1).toInt();
                }
            } else if(l.startsWith("map_Kd ")) { // texture file
                QRegularExpression r("map_Kd\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if (match.hasMatch()) {
                    mat.map_Kd = match.captured(1);
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
                QRegularExpression r("mtllib\\s+([^\n]+)");
                QRegularExpressionMatch match = r.match(l);

                if (match.hasMatch()) {
                    loadMaterial(prefix, match.captured(1));
                }
            }
            else if (l.startsWith("usemtl ")) // vertex
            {
                // v -3.4101800e-003 1.3031957e-001 2.1754370e-002
                QRegularExpression r("usemtl\\s+([^\n]+)");
                QRegularExpressionMatch match = r.match(l);
                if (match.hasMatch()) {
                    QString matName = match.captured(1);
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
                QRegularExpression r("v\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);

                if (match.hasMatch()) {
                    v.push_back(QVector3D(match.captured(1).toFloat(), match.captured(2).toFloat(), match.captured(3).toFloat()));
                }
            }
            else if (l.startsWith("vt ")) // texture coords
            {
                QRegularExpression r("vt\\s+([^ ]+)\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if (match.hasMatch()) {
                    vt.push_back(QVector3D(match.captured(1).toFloat(), match.captured(2).toFloat(), 0));
                }
            }
            else if (l.startsWith("vn ")) // vertex normal
            {
                QRegularExpression r("vn\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)");
                QRegularExpressionMatch match = r.match(l);
                if (match.hasMatch()) {
                    vn.push_back(QVector3D(match.captured(1).toFloat(), match.captured(2).toFloat(), match.captured(3).toFloat()));
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
                QRegularExpression rq("^f\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)$");
                QRegularExpressionMatch match = rq.match(l);

                if (match.hasMatch())
                {
                    QString v1 = match.captured(1);
                    QString v2 = match.captured(2);
                    QString v3 = match.captured(3);
                    QString v4 = match.captured(4);

                    // from quad we make 2 triangles
                    sTriangle tri1, tri2;

                    QRegularExpression r("^(\\d+)/?(\\d+)?/(\\d+)$");
                    QRegularExpressionMatch match = r.match(v1);

                    if (match.hasMatch())
                    {
                        int i1 = match.captured(1).toInt();
                        int i3 = match.captured(3).toInt();
                        bool ok;
                        int i2 = match.captured(2).toInt(&ok);
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
                    match = r.match(v2);
                    if (match.hasMatch())
                    {
                        int i1 = match.captured(1).toInt();
                        int i3 = match.captured(3).toInt();
                        bool ok;
                        int i2 = match.captured(2).toInt(&ok);
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
                    match = r.match(v3);
                    if (match.hasMatch())
                    {
                        int i1 = match.captured(1).toInt();
                        int i3 = match.captured(3).toInt();
                        bool ok;
                        int i2 = match.captured(2).toInt(&ok);
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
                    match = r.match(v4);
                    if (match.hasMatch())
                    {
                        int i1 = match.captured(1).toInt();
                        int i3 = match.captured(3).toInt();
                        bool ok;
                        int i2 = match.captured(2).toInt(&ok);
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
                QRegularExpression r("^f\\s+([^ ]+)\\s+([^ ]+)\\s+([^ ]+)$");
                match = r.match(l);

                if (match.hasMatch())
                {
                    QString v1 = match.captured(1);
                    QString v2 = match.captured(2);
                    QString v3 = match.captured(3);

                    sTriangle tri;

                    QRegularExpression r("^(\\d+)/?(\\d+)?/(\\d+)$");
                    QRegularExpressionMatch match = r.match(v1);

                    if (match.hasMatch())
                    {
                        int i1 = match.captured(1).toInt();
                        int i3 = match.captured(3).toInt();
                        bool ok;
                        int i2 = match.captured(2).toInt(&ok);
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
                    match = r.match(v2);
                    if (match.hasMatch())
                    {
                        int i1 = match.captured(1).toInt();
                        int i3 = match.captured(3).toInt();
                        bool ok;
                        int i2 = match.captured(2).toInt(&ok);
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
                    match = r.match(v3);
                    if (match.hasMatch())
                    {
                        int i1 = match.captured(1).toInt();
                        int i3 = match.captured(3).toInt();
                        bool ok;
                        int i2 = match.captured(2).toInt(&ok);
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

#include "terrain.h"
#include "utils.h"
#include "defines.h"

Terrain::Terrain()
{
    initializeOpenGLFunctions();
}

Terrain::~Terrain()
{
}

void Terrain::init()
{
    createTexture(mTerrainTexture, QImage(":/textures/terrain/grid.jpg"));
    createTexture(mHeightMapTexture, QImage(":/textures/terrain/grid.jpg"));
    createTexture(mNormalMapTexture, QImage(":/textures/terrain/grid.jpg"));
    //createTexture(mHeightMapTexture, QImage(":/textures/terrain/voronoi.png"));
    //createTexture(mNormalMapTexture, QImage(":/textures/terrain/voronoi_normal.png"));

    setShader(QOpenGLShader::Vertex, ":/shaders/terrain_vs.glsl");
    setShader(QOpenGLShader::TessellationControl, ":/shaders/terrain_tcs.glsl");
    setShader(QOpenGLShader::TessellationEvaluation, ":/shaders/terrain_tes.glsl");
    setShader(QOpenGLShader::Fragment, ":/shaders/terrain_fs.glsl");

    initShaders();

    initTerrainPatchesAndVBO();
}

void Terrain::createTexture(GLuint& texture, const QImage& img)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // mipmap linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // wrap mode repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(img.isNull()) {
        qd << "Failed to load terrain texture";
		return;
	}

    // assign image data to texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Terrain::render(const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector3D& lightObjPosition,
    const QVector3D& eyePos, const ShadowMap& shadowMap)
{
    // TODO: finish adding shadow map

    auto ctx = QOpenGLContext::currentContext();
    QOpenGLFunctions* f = ctx->functions();

    mVBO.bind();
    if (mVBO.size() > 0) {
        mShader.bind();

        glActiveTexture(GL_TEXTURE0);
        f->glBindTexture(GL_TEXTURE_2D, mHeightMapTexture);

        glActiveTexture(GL_TEXTURE1);
        f->glBindTexture(GL_TEXTURE_2D, mNormalMapTexture);

        glActiveTexture(GL_TEXTURE2);
        f->glBindTexture(GL_TEXTURE_2D, mTerrainTexture);

        // important to bind the shadow map texture!
        glActiveTexture(GL_TEXTURE3);
        f->glBindTexture(GL_TEXTURE_2D, shadowMap.shadowMapTexture()); // get the previously rendered shadow map depth texture

        mShader.setUniformValue("qt_shadowMapMatrix", shadowMap.shadowMapMatrix()); // light projection matrix

        QMatrix4x4 terrainModelMatrix;
        terrainModelMatrix.translate(-(TERRAIN_WIDTH * TERRAIN_SCALER) / 2.0, 0, -(TERRAIN_WIDTH * TERRAIN_SCALER) / 2.0);

        mShader.setUniformValue("qt_modelMatrix", terrainModelMatrix);
        mShader.setUniformValue("qt_viewMatrix", viewMatrix);
        mShader.setUniformValue("qt_projectionMatrix", projectionMatrix);

        mShader.setUniformValue("qt_normalMatrix", viewMatrix.normalMatrix()); // inverse & transpose of world matrix

        mShader.setUniformValue("qt_lightPositionView", viewMatrix.map(lightObjPosition));
        mShader.setUniformValue("qt_eyeVector", eyePos);

        // wireframe on
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        f = ctx->functions();
        f->glEnableVertexAttribArray(0);
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glDrawArrays(GL_PATCHES, 0, mVBO.size() / 3 / sizeof(GLfloat));

        // wireframe off
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // do i need to unbind textures?
        for (int i = 0; i < 3; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            f->glBindTexture(GL_TEXTURE_2D, 0);
        }

        mShader.release();
    }
    mVBO.release();


}



void Terrain::setShader(const QOpenGLShader::ShaderType shaderType, const QString& file)
{
    mShaderPrograms.insert(shaderType, file);
}

void Terrain::initShaders()
{
    if (mShaderPrograms.empty()) {
        qd << "No shader programs to compile!";
        return;
    }

    setlocale(LC_NUMERIC, "C");

    foreach(QOpenGLShader::ShaderType shaderType, mShaderPrograms.keys()) {
        if (!mShader.addShaderFromSourceFile(shaderType, mShaderPrograms.value(shaderType))) {
            qd << "Error compiling shader!";
            return;
        }
    }

    mShader.setPatchVertexCount(4);
    
    if (!mShader.link()) {
        qd << "Failed to link shader programs!";
        return;
    }

    if (!mShader.bind()) {
        qd << "Failed to link shader programs!";
        return;
    }

    mShader.release();

    setlocale(LC_ALL, "");
}

void Terrain::initTerrainPatchesAndVBO()
{
    // patches
    QVector<QVector3D> terrainPatchData;
    for (int x = 0; x < TERRAIN_WIDTH; x++) {
        for (int z = 0; z < TERRAIN_WIDTH; z++) {

            QMatrix4x4 m;

            m.translate(QVector3D(x * TERRAIN_SCALER, 0, z * TERRAIN_SCALER));
            m.scale(QVector3D(TERRAIN_SCALER, 0, TERRAIN_SCALER));

            terrainPatchData.push_back(m.map(QVector3D(0, 0, 0)));
            terrainPatchData.push_back(m.map(QVector3D(1, 0, 0)));
            terrainPatchData.push_back(m.map(QVector3D(1, 0, 1)));
            terrainPatchData.push_back(m.map(QVector3D(0, 0, 1)));

            QVectorIterator<QVector3D> it(terrainPatchData);
            while (it.hasNext())
            {
                QVector3D v = it.next();
                mData.append(v.x());
                mData.append(v.y());
                mData.append(v.z());
            }
        }
    }

    if(!mData.size())
		return;

    // create vbo
    qd << "Terrain VBO create: " << ((mVBO.create() == true) ? "success" : "failed");
    qd << "Terrain VBO bind: " << ((mVBO.bind() == true) ? "success" : "failed");
    mVBO.allocate(mData.constData(), mData.size() * sizeof(GLfloat));
    qd << "VBO size:" << mVBO.size() << mVBO.size() / sizeof(GLfloat);
    mVBO.release();
}

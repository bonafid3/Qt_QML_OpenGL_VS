#include "cmesh.h"
#include "utils.h"
#include <QLocale>

cMesh::cMesh() : mParent(nullptr)
{
}

cMesh::cMesh(const QString& file, cMesh* parent) : mParent(parent), mFile(file)
{
}

void cMesh::loadMesh()
{
    if (mFile.isEmpty()) {
        qd << "No file to load!";
        return;
    }

    if(mFile.toLower().endsWith(".obj")) {
		mMeshLoader = new cObjLoader();
	} if(mFile.toLower().endsWith(".stl")) {
		mMeshLoader = new cStlLoader();
	}
	mMeshLoader->load(mFile);
	qd << "triangles" << mMeshLoader->mTriangles.size();
}

void cMesh::loadMesh(const QString &file)
{
    mFile = file;
    loadMesh();
}

void cMesh::setShader(const QOpenGLShader::ShaderType shaderType, const QString &file)
{
    mShaderPrograms.insert(shaderType, file);
}

void cMesh::setTexture(const QString &file)
{
    if(mTexture) delete mTexture;
    mTexture = new QOpenGLTexture(QImage(file));
    mTexture->setMinificationFilter(QOpenGLTexture::Linear);
    mTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    mTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
}

void cMesh::initTextures()
{
    cObjLoader *objLoader;
    if((objLoader = dynamic_cast<cObjLoader*>(mMeshLoader))!=nullptr)
    {
        foreach(sMaterial mat, objLoader->mMaterials) {
            if(mat.map_Kd.size() > 0)
                setTexture(":/"+mat.map_Kd);
        }
    }
}

QMatrix4x4& cMesh::baseTransform()
{
    return mBaseTransform;
}

void cMesh::init()
{
	//initVAO();
    initVBO();
    qd<<"glerror"<<glGetError();
    initTextures();
    qd<<"glerror"<<glGetError();
    initShaders();
    qd<<"glerror"<<glGetError();
}

void cMesh::initVAO()
{
	mVAO.create();
}

void cMesh::initVBO()
{
	if(!mMeshLoader->triangles().size()) {
		qd << "No trianges to init VBO!";
		return;
	}

    if(mVBO.isCreated()) {
        mVBO.destroy();
    }

	mVBO.create();

	if(!mVBO.isCreated()) {
		qd<<"shit, VBO is not created, glerror"<<glGetError();
		return;
	}

	qd << "triangles size" << sizeof(sTriangle);
	qd << "binding to mesh vbo" << mVBO.bind();
	mVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	mVBO.allocate(mMeshLoader->triangles().constData(), mMeshLoader->triangles().size() * sizeof(sTriangle));
    mVBO.release();
}

void cMesh::initShaders()
{
    if(mShaderPrograms.empty()) {
        qd << "No shader programs to compile!";
        return;
    }

    //setlocale(LC_NUMERIC, "C");

    foreach(QOpenGLShader::ShaderType shaderType, mShaderPrograms.keys()) {
        if (!mShader.addShaderFromSourceFile(shaderType, mShaderPrograms.value(shaderType))) {
            qd << "Error compiling shader!";
            return;
        }
    }

    if (!mShader.link()) {
        qd << "Failed to link shader programs!";
        return;
    }

    if (!mShader.bind()) {
        qd << "Failed to link shader programs!";
        return;
    }

    mShader.release();
    //setlocale(LC_ALL, "");
}

QMatrix4x4& cMesh::localTransform()
{
    return mLocalTransform;
}

QMatrix4x4 cMesh::globalTransform()
{
    QMatrix4x4 result;
    if (mParent) {
        result = mParent->localTransform();
    }
    return result * localTransform();
}

QVector3D cMesh::position()
{
    return localTransform().map(QVector3D(0,0,0));
}

QOpenGLShaderProgram &cMesh::shader()
{
    return mShader;
}

QOpenGLTexture& cMesh::texture()
{
    return *mTexture;
}

QOpenGLBuffer& cMesh::vbo()
{
	return mVBO;
}

QOpenGLVertexArrayObject& cMesh::vao()
{
	return mVAO;
}

int cMesh::triangleCount()
{
	return mMeshLoader->triangles().size();
}

#ifndef C3DOBJECT_H
#define C3DOBJECT_H

#include <QMap>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include "cstlloader.h"
#include "cobjloader.h"

class cMesh
{
public:
	cMesh();
	cMesh(const QString& file, cMesh* parent=nullptr);
	void loadMesh();
    void loadMesh(const QString& file);
    void setTexture(const QString& file);
    void setShader(const QOpenGLShader::ShaderType shaderType, const QString& file);

    void init();
	void initVAO();
	void initVBO();
    void initShaders();
	void initTextures();
	QMatrix4x4& baseTransform();
	QMatrix4x4& localTransform();
	QMatrix4x4 globalTransform();

    QVector3D position();

	QOpenGLShaderProgram& shader();
	QOpenGLTexture& texture();
	QOpenGLBuffer& vbo();

	QOpenGLVertexArrayObject& vao();

	int triangleCount();
private:
	cMesh* mParent = nullptr;
	
	QString mFile;
	QOpenGLBuffer mVBO;
	QOpenGLVertexArrayObject mVAO;
	cAbstractMeshLoader *mMeshLoader=nullptr;
    QOpenGLTexture *mTexture=nullptr;
	QMatrix4x4 mBaseTransform;
	QMatrix4x4 mLocalTransform;
    QMap<QOpenGLShader::ShaderType, QString> mShaderPrograms;
    QOpenGLShaderProgram mShader;

};

#endif // C3DOBJECT_H

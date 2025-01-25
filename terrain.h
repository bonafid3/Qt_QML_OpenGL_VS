#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMap>
#include <QOpenGLBuffer>
#include <QImage>

#include "shadowmap.h"

class Terrain : public QOpenGLFunctions
{
public:
	Terrain();
	~Terrain();

	void initShaders();
	void initTerrainPatchesAndVBO();
	void setShader(const QOpenGLShader::ShaderType shaderType, const QString& file);
	void init();

	void createTexture(GLuint& texture, const QImage& img);

	void render(const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector3D& lightObjPosition,
		const QVector3D& eyePos, const ShadowMap& shadowMap);

private:
	QMap<QOpenGLShader::ShaderType, QString> mShaderPrograms;
	QOpenGLShaderProgram mShader;
	QVector<GLfloat> mData;
	QOpenGLBuffer mVBO;

	GLuint mTerrainTexture;
	GLuint mHeightMapTexture;
	GLuint mNormalMapTexture;
};

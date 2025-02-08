#pragma once

#include <QOpenGLFunctions>
#include <QMatrix4x4>

#include <QOpenGLShaderProgram>

#include "defines.h"
#include "cmesh.h"

class SSAO : public QOpenGLFunctions
{
public:
	SSAO();
	~SSAO();

	void init(const QSize& dimensions);

	void createSampleKernel();
	void createNoiseData();
	void initTextures(const QSize& dimensions);
	void render(const QSize& dimensions, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector<cMesh*>& entities, GLuint originalFBOHandle);

	void renderSSAO(const QSize& dimensions, const QMatrix4x4& projectionMatrix, GLuint originalFBOHandle);

	void renderSSAOBlur(const QSize& dimensions, GLuint originalFBOHandle);

	const GLuint& geometryTexture() const;
	const GLuint& normalTexture() const;
	const GLuint& albedoTexture() const;
	const GLuint& SSAOTexture() const;
	const GLuint& SSAOBlurTexture() const;
	const GLuint& noiseTexture() const;

	QOpenGLShaderProgram mPrePassShader;
	QOpenGLShaderProgram mSSAOShader;
	QOpenGLShaderProgram mSSAOBlurShader;

private:
	cMesh mMesh;
	GLuint mGeometryBufferFBO=0, mSSAOFBO=0, mSSAOBlurFBO=0;
	GLuint mGeometryBufferTexture=0, mNormalBufferTexture=0, mAlbedoTexture=0, mSSAOTexture=0, mNoiseTexture=0, mSSAOBlurTexture=0;

	GLuint mDepthBuffer=0;

	QVector<QVector3D> mSSAOKernel;
	QVector<QVector3D> mSSAONoise;

};

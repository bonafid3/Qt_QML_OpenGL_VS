#pragma once

#include <QOpenGLFunctions>

#include "cmesh.h"

class Skybox : public QOpenGLFunctions
{
public:
	Skybox();
	~Skybox();

	void init();
	void setupTextures(const QStringList& images);

	void render(const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector3D eye);

private:
	cMesh mMesh;
	GLuint mSkyBoxTexture;
};

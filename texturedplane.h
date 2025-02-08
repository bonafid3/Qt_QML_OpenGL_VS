#pragma once

#include <QOpenGLFunctions>

#include "cmesh.h"

class TexturedPlane : public QOpenGLFunctions
{
public:
	TexturedPlane();
	~TexturedPlane();

	void init();

	void render(const QSize& sceneSize, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const GLuint shadowMapTexture);

private:
	cMesh mMesh;
};

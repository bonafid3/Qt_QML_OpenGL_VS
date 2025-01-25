#pragma once

#include <QOpenGLFunctions>
#include <QMatrix4x4>

#include "defines.h"
#include "cmesh.h"

class ShadowMap : public QOpenGLFunctions
{
public:
	ShadowMap();
	~ShadowMap();

	void init();
	void render(const QSize& sceneSize, const QVector3D& lightObjPosition, const QVector<cMesh*>& entities, GLuint originalFBOHandle);

	const GLuint& shadowMapTexture() const;
	const QMatrix4x4& shadowMapMatrix() const;

	void saveShadowMap(const QString& fileName);

private:
	cMesh mMesh;
	GLuint mShadowMapFBO, mShadowMapTexture;

	QMatrix4x4 mShadowMapMatrix;
};

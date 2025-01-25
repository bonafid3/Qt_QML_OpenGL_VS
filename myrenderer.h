#pragma once

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <QOpenGLVertexArrayObject>

#include <QVector3D>
#include <QMatrix4x4>

#include "defines.h"

#include "enums.h"
#include "cmesh.h"
#include "reactionwheel.h"

#include "skybox.h"
#include "shadowmap.h"
#include "terrain.h"

class MyRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
public:
	MyRenderer();
	~MyRenderer() = default;

	QOpenGLFramebufferObject* createFramebufferObject(const QSize& s);
	virtual void synchronize(QQuickFramebufferObject *fbo);
	void render();

	void renderObjects(const QMatrix4x4& viewMatrix, QVector<cMesh*> meshes);

	void mousePositionChanged(float mouseX, float mouseY);
	bool screenToWorld(const QPoint& winCoord, int z_ndc, QVector3D &pw);

private:

	QOpenGLFramebufferObject* mGraphicsLayerFBO;

	Skybox mSkybox;
	ShadowMap mShadowMap;
	Terrain mTerrain;

	cMesh mLightObj;
	cMesh mMill;
	ReactionWheel mReactionWheel;

	QOpenGLShaderProgram mLineShader;

	QQuickWindow* mWindow = nullptr;

	QMatrix4x4 mProjectionMatrix;
	QSize mDimensions;

	QVector3D mEye;
	QVector3D mView;
	QVector3D mUp;

	bool mPerspectiveProjection = false;

	int mMouseX = 0;
	int mMouseY = 0;

	bool mLeftMouseButtonPressed = false;
	bool mRightMouseButtonPressed = false;

	QPoint mLeftMouseButtonPressCoord;
	QPoint mRightMouseButtonPressCoord;

	std::vector<QVector3D> mSearchRadiusPoints;
	std::vector<QVector3D> mSearchRadiusColors;

	void initShader(QOpenGLShaderProgram &shaderProgram, const QString &vs, const QString &gs, const QString &fs);
};

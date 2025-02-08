#pragma once

#include <QOpenGLFunctions>

#include "cmesh.h"
#include "shadowmap.h"
#include "ssao.h"

#include <vector>

class ReactionWheel : public QOpenGLFunctions
{
public:
	ReactionWheel();
	~ReactionWheel();

	void init();

	void render(const QSize& sceneSize, const ShadowMap& shadowMap, const SSAO& ssao, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector3D& lightObjPosition, const QVector3D& eyePosition);

	QVector<cMesh*> entities() { return mEntities; }

	void setArmAngle(const float angle);
	float armAngle() const;

	void setWheelSpeed(const float speed);
	float wheelSpeed() const;

	void setWheelAngle(const float angle);
	float wheelAngle() const;

private:
	cMesh mBaseMesh;
	cMesh mArmMesh;
	cMesh mWheelMesh;

	float mArmAngle=0.0f;
	float mWheelAngle=0.0f;
	float mWheelSpeed=0.0f;

	QVector<cMesh*> mEntities;
};

#include "reactionwheel.h"

ReactionWheel::ReactionWheel() :
	mBaseMesh{ ":/meshes/react_stand.obj" },
	mArmMesh{ ":/meshes/react_arm.obj", &mBaseMesh },
	mWheelMesh{ ":/meshes/react_wheel.obj", &mArmMesh }
{
	initializeOpenGLFunctions();

	mEntities.push_back(&mWheelMesh);
	mEntities.push_back(&mArmMesh);
	mEntities.push_back(&mBaseMesh);

	mBaseMesh.baseTransform().translate(QVector3D(0, 0, 0)); // no need to translate the base
	mArmMesh.baseTransform().translate(QVector3D(0, 1.155, -0.125));
	mWheelMesh.baseTransform().translate(QVector3D(0, 10.5, 0.5));

	setArmAngle(0);
	setWheelAngle(0);
}

ReactionWheel::~ReactionWheel()
{
}

void ReactionWheel::init()
{
	mBaseMesh.loadMesh();
	mBaseMesh.setShader(QOpenGLShader::Vertex, ":/shaders/object_vs.glsl");
	mBaseMesh.setShader(QOpenGLShader::Fragment, ":/shaders/object_fs.glsl");
	mBaseMesh.init();

	mArmMesh.loadMesh();
	mArmMesh.setShader(QOpenGLShader::Vertex, ":/shaders/object_vs.glsl");
	mArmMesh.setShader(QOpenGLShader::Fragment, ":/shaders/object_fs.glsl");
	mArmMesh.init();

	mWheelMesh.loadMesh();
	mWheelMesh.setShader(QOpenGLShader::Vertex, ":/shaders/object_vs.glsl");
	mWheelMesh.setShader(QOpenGLShader::Fragment, ":/shaders/object_fs.glsl");
	mWheelMesh.init();
}

void ReactionWheel::render(const QSize& sceneSize, const ShadowMap& shadowMap, const SSAO& ssao, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector3D& lightObjPosition, const QVector3D& eyePosition)
{
	auto ctx = QOpenGLContext::currentContext();
	QOpenGLFunctions* f = ctx->functions();

	glViewport(0, 0, sceneSize.width(), sceneSize.height());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap.shadowMapTexture());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ssao.SSAOBlurTexture());

	for (auto&& entity : entities()) {
		entity->vbo().bind();
		entity->shader().bind();

		entity->shader().setUniformValue("qt_shadowMapMatrix", shadowMap.shadowMapMatrix()); // light projection matrix

		entity->shader().setUniformValue("qt_modelMatrix", entity->globalTransform());
		entity->shader().setUniformValue("qt_viewMatrix", viewMatrix);
		entity->shader().setUniformValue("qt_projectionMatrix", projectionMatrix);

		QMatrix4x4 modelViewMatrix = viewMatrix * entity->globalTransform();
		entity->shader().setUniformValue("qt_normalMatrix", modelViewMatrix.normalMatrix()); // inverse & transpose of world matrix

		entity->shader().setUniformValue("qt_lightPositionView", viewMatrix.map(lightObjPosition)); // for diffuse

		//qd << "eyePosition: " << eyePosition << (viewMatrix * eyePosition);
		entity->shader().setUniformValue("qt_eyePositionView", QVector3D(0,0,0)); // same as viewMatrix * eyePosition

		f->glEnableVertexAttribArray(0);
		f->glEnableVertexAttribArray(1);
		f->glEnableVertexAttribArray(2);

		f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
		f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
		f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
		glDrawArrays(GL_TRIANGLES, 0, entity->vbo().size() / 9 / sizeof(GLfloat));

		entity->shader().release();
		entity->vbo().release();
	}
}

void ReactionWheel::setArmAngle(const float angle)
{
	mArmAngle = angle;
	mArmMesh.localTransform() = mArmMesh.baseTransform();
	mArmMesh.localTransform().rotate(angle, QVector3D(0, 0, 1));
}

float ReactionWheel::armAngle() const
{
	return mArmAngle;
}

void ReactionWheel::setWheelSpeed(const float speed)
{
	mWheelSpeed = speed;
}

float ReactionWheel::wheelSpeed() const
{
	return mWheelSpeed;
}

void ReactionWheel::setWheelAngle(const float angle)
{
	mWheelAngle = angle;
	mWheelMesh.localTransform() = mWheelMesh.baseTransform();
	mWheelMesh.localTransform().rotate(angle, QVector3D(0, 0, 1));
}

float ReactionWheel::wheelAngle() const
{
	return mWheelAngle;
}


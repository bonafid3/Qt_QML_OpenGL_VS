#include "myrenderer.h"

#include <QQuickWindow>

#include "graphicslayer.h"
#include "myrenderer.h"
#include "utils.h"

MyRenderer::MyRenderer() : mEye(0, 20, 50), mView(0, 0, 0), mUp(0, 1, 0)
{
	initializeOpenGLFunctions();

	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	auto f = ctx->functions();
	auto ef = ctx->extraFunctions();

	mSkybox.init();
	mShadowMap.init();
	mTerrain.init();

	/*
	mMill.loadMesh(":/meshes/mill.obj");
	mMill.setShader(QOpenGLShader::Vertex, ":/shaders/object_vs.glsl");
	mMill.setShader(QOpenGLShader::Fragment, ":/shaders/object_fs.glsl");
	mMill.localTransform().translate(QVector3D(0, 0, 0));
	mMill.init();
	*/

	mReactionWheel.init();

	mLightObj.loadMesh(":/meshes/cube.stl");
	mLightObj.setShader(QOpenGLShader::Vertex, ":/shaders/object_vs.glsl");
	mLightObj.setShader(QOpenGLShader::Fragment, ":/shaders/object_fs.glsl");
	mLightObj.localTransform().translate(QVector3D(-100, 100, 200));
	mLightObj.localTransform().scale(QVector3D(0, 0, 0));
	mLightObj.init();

	//qd << "Supported shading language version"  << QString((char*)glGetString(GL_VENDOR))<< QString((char*)glGetString(GL_RENDERER)) << QString((char*)glGetString(GL_VERSION)) << QString((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// drawing billboard like lines
	//initShader(mLineShader, ":/shaders/line_vs.glsl", ":/shaders/line_gs.glsl", ":/shaders/line_fs.glsl");

	// displaying the images
	//initShader(mTexturedQuadShader, ":/textured_vs.glsl", ":/textured_gs.glsl", ":/textured_fs.glsl");
}

void MyRenderer::renderObjects(const QMatrix4x4& viewMatrix, QVector<cMesh*> entities)
{
	auto ctx = QOpenGLContext::currentContext();
	QOpenGLFunctions* f = ctx->functions();

	glViewport(0, 0, mDimensions.width(), mDimensions.height());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mShadowMap.shadowMapTexture());

	for (auto&& entity : entities) {
		QMatrix4x4 modelViewMatrix = viewMatrix * entity->localTransform();

		entity->vbo().bind();
		entity->shader().bind();

		entity->shader().setUniformValue("qt_shadowMapMatrix", mShadowMap.shadowMapMatrix()); // light projection matrix

		entity->shader().setUniformValue("qt_modelMatrix", entity->localTransform());
		entity->shader().setUniformValue("qt_viewMatrix", viewMatrix);
		entity->shader().setUniformValue("qt_projectionMatrix", mProjectionMatrix);

		entity->shader().setUniformValue("qt_normalMatrix", modelViewMatrix.normalMatrix()); // inverse & transpose of world matrix

		entity->shader().setUniformValue("qt_lightPositionView", viewMatrix * mLightObj.position()); // for diffuse

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

void MyRenderer::initShader(QOpenGLShaderProgram& shaderProgram, const QString& vs, const QString& gs, const QString& fs)
{
	if(!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, vs)) {
		qd <<"Vertex shader compilation error";
		exit(0);
	}

	if(gs.size())
	{
		if(!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Geometry, gs)) {
			qd <<"Geometry shader compilation error";
			exit(0);
		}
	}

	if(!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, fs)) {
		qd <<"Fragment shader compilation error";
		exit(0);
	}

	if(!shaderProgram.link()) {
		qd <<"Shader program linker error";
		exit(0);
	}
}

QOpenGLFramebufferObject* MyRenderer::createFramebufferObject(const QSize& s)
{
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setSamples(4); // not sure if this works!
	mGraphicsLayerFBO = new QOpenGLFramebufferObject(s, format);
	return mGraphicsLayerFBO;
}

void MyRenderer::synchronize(QQuickFramebufferObject *fbo)
{
	GraphicsLayer* graphicsLayer = static_cast<GraphicsLayer*>(fbo);
	if(graphicsLayer == nullptr)
		return;

	graphicsLayer->fpsCounter().value<FPSCounter*>()->update();

	mWindow = graphicsLayer->window();

	if(graphicsLayer->mCameraCommands.size())
	{
		mEye = graphicsLayer->mCameraCommands.back().mEye;
		mView = graphicsLayer->mCameraCommands.back().mView;
		mUp = graphicsLayer->mCameraCommands.back().mUp;
		graphicsLayer->mCameraCommands.pop_back();
	}

	int w = graphicsLayer->width(), h = graphicsLayer->height();
	mDimensions = QSize(w, h);

	const float FOV = 45.0f;
	const float aspect = (float) w / (float) h;

	mPerspectiveProjection = graphicsLayer->perspective();

	mProjectionMatrix.setToIdentity();
	mProjectionMatrix.perspective(FOV, aspect, ZNEAR, ZFAR);

	// get the texture if loaded
	if(graphicsLayer->syncRequired())
	{
		graphicsLayer->setSyncRequired(false);
	} // syncRequired
	
	// SEARCH RADIUS
	mSearchRadiusPoints.clear();
	mSearchRadiusColors.clear();
	const int SEG = 64;
	const float searchRadius = 0.1;
	for (int i = 0; i <= SEG; i++)
	{
		float aStep = 2 * M_PI / SEG;
		float angle = aStep * i;
		float xp = cos(angle) * searchRadius;
		float yp = sin(angle) * searchRadius;

		mSearchRadiusPoints.push_back(QVector3D(((float(xp))), ((float(yp))), 0));
		mSearchRadiusColors.push_back(QVector3D(0.1f, 0.1f, 0.1f));
	}
	
	//mSearchRadiusPoints.push_back(QVector3D(0, 0, 0));
	//mSearchRadiusColors.push_back(QVector3D(0.1f, 0.1f, 0.1f));

	//mSearchRadiusPoints.push_back(QVector3D(0.1, 0.1, 0));
	//mSearchRadiusColors.push_back(QVector3D(0.1f, 0.1f, 0.1f));

	float speed = 1.0f;
	QVector3D eyeDir = (mView-mEye).normalized();

	/////////////
	// LEFT CLICK
	/////////////
	if (mLeftMouseButtonPressed != graphicsLayer->leftMouseButtonPressed())
	{
		mLeftMouseButtonPressed = graphicsLayer->leftMouseButtonPressed();
		mLeftMouseButtonPressCoord = graphicsLayer->leftMouseButtonPressPosition();

		if (mLeftMouseButtonPressed == false && // watch for release when no change in mouse position, which is a click
			mLeftMouseButtonPressCoord == graphicsLayer->mousePosition())
		{
			QVector3D pw;
			if (screenToWorld(mLeftMouseButtonPressCoord, -1.0, pw)) {
				qd << "left click at" << pw;
			}
		}
	}

	if(mMouseX != graphicsLayer->mousePosition().x() || mMouseY != graphicsLayer->mousePosition().y())
	{
		mMouseX = graphicsLayer->mousePosition().x();
		mMouseY = graphicsLayer->mousePosition().y();

		mousePositionChanged(mMouseX, mMouseY);

		//if(screenToWorld(QPoint(mMouseX, mMouseY), -1.0, mCachedMousePos))
			//graphicsLayer->updateClosestNailIndex(mCachedMousePos);
	}

	if(graphicsLayer->mWheelValue.size())
	{
		auto amt = graphicsLayer->mWheelValue.front() / 120.0f;
		graphicsLayer->mWheelValue.pop_front();

		QVector3D pw;
		if(screenToWorld(QPoint(mMouseX, mMouseY), -1.0, pw))
		{
			QVector3D wheelDir = (pw - mEye).normalized();
			mEye += -wheelDir * speed * amt;
			mView += -wheelDir * speed * amt;
		}
		else
		{
			mEye += -eyeDir * speed * amt;
			mView += -eyeDir * speed * amt;
		}
	}

	if(graphicsLayer->mKeyDown[Qt::Key_Left])
	{
		mReactionWheel.setArmAngle(mReactionWheel.armAngle() + 1);
	}

	if(graphicsLayer->mKeyDown[Qt::Key_Right])
	{
		mReactionWheel.setArmAngle(mReactionWheel.armAngle() - 1);
	}

	if (graphicsLayer->mKeyDown[Qt::Key_Up])
	{
		mReactionWheel.setWheelSpeed(mReactionWheel.wheelSpeed() + 1);
	}

	if (graphicsLayer->mKeyDown[Qt::Key_Down])
	{
		mReactionWheel.setWheelSpeed(mReactionWheel.wheelSpeed() - 1);
	}

	if(graphicsLayer->mKeyDown[Qt::Key_W])
	{
		mEye += eyeDir * speed;
		mView += eyeDir * speed;
	}

	if(graphicsLayer->mKeyDown[Qt::Key_S])
	{
		mEye += -eyeDir * speed;
		mView += -eyeDir * speed;
	}

	if(graphicsLayer->mKeyDown[Qt::Key_A])
	{
		QVector3D xaxis = QVector3D::crossProduct(mUp, eyeDir).normalized();
		mEye += xaxis * speed;
		mView += xaxis * speed;
	}

	if(graphicsLayer->mKeyDown[Qt::Key_D])
	{
		QVector3D xaxis = QVector3D::crossProduct(mUp, eyeDir).normalized();
		mEye += -xaxis * speed;
		mView += -xaxis * speed;
	}

	if(graphicsLayer->mKeyDown[Qt::Key_E])
	{
		mEye += mUp * speed;
		mView += mUp * speed;
	}

	if(graphicsLayer->mKeyDown[Qt::Key_Q])
	{
		mEye += -mUp * speed;
		mView += -mUp * speed;
	}
}

void MyRenderer::mousePositionChanged(float mouseX, float mouseY)
{
	QVector3D eyeDir = (mView-mEye).normalized();

	if(mLeftMouseButtonPressed)
	{
		QQuaternion result;

		float xangle = d2r(mLeftMouseButtonPressCoord.x() - mouseX) * 0.1f;
		float yangle = -d2r(mLeftMouseButtonPressCoord.y() - mouseY) * 0.1f;

		mLeftMouseButtonPressCoord.setX(mouseX);
		mLeftMouseButtonPressCoord.setY(mouseY);

		QVector3D yaxis = QVector3D(0,1,0); // rotate around the world's up vector
		QQuaternion r_quat(cos(xangle/2), yaxis.x()*sin(xangle/2), yaxis.y()*sin(xangle/2), yaxis.z()*sin(xangle/2));
		r_quat.normalize();
		QQuaternion v_quat(0, eyeDir); // create view quaternion from view vector
		QQuaternion u_quat(0, mUp);
		result = (r_quat * v_quat) * r_quat.conjugated();
		QVector3D eyeShit = QVector3D(result.x(), result.y(), result.z()).normalized();
		mView = mEye + eyeShit;

		result = (r_quat * u_quat) * r_quat.conjugated();
		mUp = QVector3D(result.x(), result.y(), result.z()).normalized();
		//qd << "eye" << mEye << "view" << mView << "up" << mUp << "eyeDir" << eyeDir;

		// new eyedir based on the previous rotation about the vertical camera axis
		eyeDir = (mView-mEye).normalized();

		// rotation axis
		QVector3D xaxis = QVector3D::crossProduct(mUp, eyeDir).normalized();

		QQuaternion rr_quat(cos(xangle/2), xaxis.x()*sin(yangle/2), xaxis.y()*sin(yangle/2), xaxis.z()*sin(yangle/2));
		rr_quat.normalize();
		QQuaternion vv_quat(0, eyeDir); // create view quaternion from view vector
		QQuaternion uu_quat(0, mUp);
		result = (rr_quat * vv_quat) * rr_quat.conjugated();
		eyeShit = QVector3D(result.x(), result.y(), result.z()).normalized();
		mView = mEye + eyeShit;

		result = (rr_quat * uu_quat) * rr_quat.conjugated();
		mUp = QVector3D(result.x(), result.y(), result.z()).normalized();

		//qd << "quat res" << result;
	}

	update();
}

//////////
/// RENDER
//////////

void MyRenderer::render()
{
	QMatrix4x4 viewMatrix;
	viewMatrix.lookAt(mEye, mView, mUp);

	glClearColor(0.9f, 0.5f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE); // qt surface format sets the number of sampling

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation( GL_FUNC_ADD );

	mReactionWheel.setWheelAngle(mReactionWheel.wheelAngle() + mReactionWheel.wheelSpeed());

	glViewport(0, 0, mDimensions.width(), mDimensions.height());

	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	auto f = ctx->functions();
	auto ef = ctx->extraFunctions();

	mSkybox.render(viewMatrix, mProjectionMatrix, mEye);

	// important to restore the graphics layer framebuffer after rendering the shadow map
	mShadowMap.render(mDimensions, mLightObj.position(), mReactionWheel.entities(), mGraphicsLayerFBO->handle());

	f->glViewport(0, 0, mDimensions.width(), mDimensions.height());

	mTerrain.render(viewMatrix, mProjectionMatrix, mLightObj.position(), mEye, mShadowMap);

	mReactionWheel.render(mDimensions, mShadowMap, viewMatrix, mProjectionMatrix, mLightObj.position());

	/*
	/// DRAW LINES
	if (mSearchRadiusPoints.size())
	{
		mLineShader.bind();
		mLineShader.setUniformValue("qt_modelViewMatrix", viewMatrix);
		mLineShader.setUniformValue("qt_projectionMatrix", mProjectionMatrix);
		mLineShader.setUniformValue("qt_cameraPos", mEye);

		mLineShader.setUniformValue("qt_lineWidth", 0.005f);

		mLineShader.enableAttributeArray(0);
		mLineShader.enableAttributeArray(1);
		mLineShader.setAttributeArray(0, GL_FLOAT, mSearchRadiusPoints.data(), 3);
		mLineShader.setAttributeArray(1, GL_FLOAT, mSearchRadiusColors.data(), 3);
		glDrawArrays(GL_LINE_STRIP, 0, mSearchRadiusPoints.size());
		mLineShader.disableAttributeArray(0);
		mLineShader.disableAttributeArray(1);
		mLineShader.release();
	}
	*/

	// this is not necessary, but it is good practice to unbind the framebuffer
	mWindow->resetOpenGLState();
}

bool MyRenderer::screenToWorld(const QPoint& winCoord, int z_ndc, QVector3D &pw)
{
	// define the plane
	sPlane plane(QVector3D(0,0,1), QVector3D(0,0,0));

	QMatrix4x4 modelViewMatrix;

	QVector4D ray_clip((2.0f * winCoord.x()) / mDimensions.width() - 1.0f, (2.0f * winCoord.y()) / mDimensions.height() - 1.0f, z_ndc, 1.0);
	QVector4D ray_eye = mProjectionMatrix.inverted() * ray_clip;

	ray_eye.setZ(-1);
	ray_eye.setW(0);

	modelViewMatrix.lookAt(mEye, mView, mUp);
	QVector3D rw = QVector4D(modelViewMatrix.inverted() * ray_eye).toVector3D();

	float c, d = QVector3D::dotProduct(plane.n, plane.p);

	QVector3D rayDir = (mView - mEye).normalized();

	if(mPerspectiveProjection) {
		c = QVector3D::dotProduct(plane.n, mEye) - d;
		if(c < 0) { return false; } // we are behind the plane
	} else {
		rw += mEye;
		c = QVector3D::dotProduct(plane.n, rw) - d;
	}

	float a;
	if(mPerspectiveProjection) {
		a = QVector3D::dotProduct(plane.n, rw);
	} else {
		a = QVector3D::dotProduct(plane.n, rayDir);
	}

	if(a==0) { return false; } // ray is parallel to the plane

	float t = c / a;

	if(t==0) return false;

	if(mPerspectiveProjection) {
		pw = mEye - rw * t;
	} else {
		pw = rw - rayDir * t;
	}

	//qd << "ray clip" << ray_clip << "ray eye" << ray_eye << "mEye" << mEye << rw.normalized();
	//qd << "ray_world" << rw << "point in world" << pw << "rw*t" << rw*t << "t" << t;

	return true;
}

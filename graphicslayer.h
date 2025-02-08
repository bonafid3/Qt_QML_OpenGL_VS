#pragma once

#include <QDebug>
#include <QQuickFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QOpenGLTexture>

#include <QtConcurrent/QtConcurrent>

#include <list>

#include "myrenderer.h"
#include "enums.h"
#include "fpscounter.h"

struct sCamera
{
	QVector3D mEye;
	QVector3D mView;
	QVector3D mUp;
};

class GraphicsLayer : public QQuickFramebufferObject
{
	Q_OBJECT
public:

	Q_PROPERTY(bool perspective                   READ perspective           WRITE setPerspective           NOTIFY perspectiveChanged)
	Q_PROPERTY(QVariant fpsCounter                READ fpsCounter                                           CONSTANT)
	//Q_PROPERTY(QStringListModel comboModel        READ comboModel                                           CONSTANT)

	GraphicsLayer();

	MyRenderer* createRenderer() const override;
	Q_INVOKABLE void resetCamera();
	MyRenderer* mRenderer = nullptr;

	QMap<int, bool> mKeyDown;
	std::list<int> mWheelValue;

	bool perspective() const;
	void setPerspective(const bool p);

	bool syncRequired() const;
	void setSyncRequired(bool newState);

	QPoint mousePosition() const;

	bool leftMouseButtonPressed() const;
	QPoint leftMouseButtonPressPosition() const;

	bool rightMouseButtonPressed() const;
	QPoint rightMouseButtonPressPosition() const;

	QVariant fpsCounter();

	Q_INVOKABLE void onKeyPressed(QVariant v);
	Q_INVOKABLE void onKeyReleased(QVariant v);

	Q_INVOKABLE void onWheeled(QVariant wheelEventVariant);

	Q_INVOKABLE void onMousePressed(QVariant vx, QVariant vy, QVariant vb);
	Q_INVOKABLE void onMouseReleased(QVariant vx, QVariant vy, QVariant vb);
	Q_INVOKABLE void onMousePositionChanged(QVariant vx, QVariant vy);

	//QSGNode* updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* nodeData) override;

	// if the item has focus these would be called
	//void keyPressEvent(QKeyEvent *e) override;
	//void keyReleaseEvent(QKeyEvent *e) override;

Q_SIGNALS:
	void perspectiveChanged();

public Q_SLOTS:
	void on_mTimer_timeout();
	void onWindowChanged(QQuickWindow* window);

private:

	friend class MyRenderer;
	std::vector<sCamera> mCameraCommands;
	bool mPerspective;
	bool mSyncRequired = false;
	QTimer* mTimer;

	mutable FPSCounter mFPSCounter;

	QPoint mMousePosition;
	QPoint mLeftMouseButtonPressPosition;
	QPoint mRightMouseButtonPressPosition;

	bool mLeftMouseButtonPressed = false;
	bool mRightMouseButtonPressed = false;


};

#include "graphicslayer.h"
#include "myrenderer.h"

#include <QQuickItem>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>

#include <cmath> // do we need this?

GraphicsLayer::GraphicsLayer() :
	mPerspective(false)
{

	this->setAcceptHoverEvents(true);
	this->setFlags(QQuickItem::ItemClipsChildrenToShape | QQuickItem::ItemHasContents | QQuickItem::ItemIsFocusScope | QQuickItem::ItemAcceptsDrops);
	this->setAcceptedMouseButtons(Qt::AllButtons);

	mTimer = new QTimer(this);
	mTimer->setObjectName("mTimer");
	connect(mTimer, SIGNAL(timeout()), this, SLOT(on_mTimer_timeout()));
	mTimer->start(15);

	QObject::connect(this, &QQuickItem::windowChanged, this, &GraphicsLayer::onWindowChanged);
}

void GraphicsLayer::on_mTimer_timeout()
{
	update();
}

void GraphicsLayer::onWindowChanged(QQuickWindow *window)
{
	/*
	if (window)
	{
		this->window()->setColor(QColor{ 0, 0, 0, 255 });
		this->window()->setClearBeforeRendering(true);
		this->window()->setPersistentOpenGLContext(true);
	}*/
}

MyRenderer* GraphicsLayer::createRenderer() const
{
	return new MyRenderer;
}

Q_INVOKABLE void GraphicsLayer::resetCamera()
{
	mCameraCommands.push_back({QVector3D(0,0,2), QVector3D(0,0,0), QVector3D(0,1,0)});
}

bool GraphicsLayer::perspective() const
{
	return mPerspective;
}

void GraphicsLayer::setPerspective(const bool p)
{
	if(mPerspective == p) return;
	mPerspective = p;
	Q_EMIT(perspectiveChanged());
}

bool GraphicsLayer::leftMouseButtonPressed() const
{
	return mLeftMouseButtonPressed;
}

QPoint GraphicsLayer::leftMouseButtonPressPosition() const
{
	return mLeftMouseButtonPressPosition;
}

bool GraphicsLayer::rightMouseButtonPressed() const
{
	return mRightMouseButtonPressed;
}

QPoint GraphicsLayer::rightMouseButtonPressPosition() const
{
	return mRightMouseButtonPressPosition;
}

QVariant GraphicsLayer::fpsCounter()
{
	return QVariant::fromValue(&mFPSCounter);
}

bool GraphicsLayer::syncRequired() const
{
	return mSyncRequired;
}

void GraphicsLayer::setSyncRequired(bool newState)
{
	mSyncRequired = newState;
}

QPoint GraphicsLayer::mousePosition() const
{
	return mMousePosition;
}

void GraphicsLayer::onKeyPressed(QVariant v)
{
	auto key = v.value<int>();
	mKeyDown[key] = true;
}

void GraphicsLayer::onKeyReleased(QVariant v)
{
	auto key = v.value<int>();
	mKeyDown[key] = false;
}

void GraphicsLayer::onWheeled(QVariant wheelEventVariant)
{
	auto we = wheelEventVariant.value<QObject*>();
	auto adv = we->property("angleDelta");

	if(adv.isValid())
	{
		auto ad = adv.value<QPoint>();
		mWheelValue.push_back(ad.y());
	}
}

void GraphicsLayer::onMousePressed(QVariant vx, QVariant vy, QVariant vb)
{
	if(vb.value<int>() == Qt::LeftButton)
	{
		mLeftMouseButtonPressed = true;
		mLeftMouseButtonPressPosition = QPoint(vx.value<int>(), vy.value<int>());
	}

	if(vb.value<int>() == Qt::RightButton)
	{
		mRightMouseButtonPressed = true;
		mRightMouseButtonPressPosition = QPoint(vx.value<int>(), vy.value<int>());
	}
}

void GraphicsLayer::onMouseReleased(QVariant vx, QVariant vy, QVariant vb)
{
	if(vb.value<int>() == Qt::LeftButton)
		mLeftMouseButtonPressed = false;

	if(vb.value<int>() == Qt::RightButton)
		mRightMouseButtonPressed = false;
}

void GraphicsLayer::onMousePositionChanged(QVariant vx, QVariant vy)
{
	mMousePosition = QPoint(vx.value<int>(), vy.value<int>());
}


/*
QSGNode* GraphicsLayer::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* nodeData)
{
	if (!node) {
		node = QQuickFramebufferObject::updatePaintNode(node, nodeData);
		QSGSimpleTextureNode* n = static_cast<QSGSimpleTextureNode*>(node);
		if (n)
			n->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
		return node;
	}
	return QQuickFramebufferObject::updatePaintNode(node, nodeData);
}
*/
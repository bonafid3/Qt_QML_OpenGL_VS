#include "craftcolumn.h"

CraftColumn::CraftColumn(QQuickItem* parent) : QQuickItem{ parent }, mSpacing{ 0.0 }
{
	setFlag(ItemHasContents, true);
}

CraftColumn::~CraftColumn()
{
}

qreal CraftColumn::spacing() const
{
	return mSpacing;
}

void CraftColumn::setSpacing(qreal s)
{
	if (mSpacing == s) return;
	mSpacing = s;

	Q_EMIT spacingChanged();

	if (isComponentComplete())
		positionItems();
}

void CraftColumn::positionItems()
{
	qreal maxImplicitWidth = 0.0;
	qreal totalImplicitHeight = 0.0;

	QList<QQuickItem*> children = childItems();
	for (int i = 0; i < children.count(); ++i) {
		QQuickItem* child = children.at(i);

		child->setY(totalImplicitHeight);

		if (child->implicitWidth() > maxImplicitWidth)
			maxImplicitWidth = child->implicitWidth();
		if (child->implicitHeight() > 0) {
			totalImplicitHeight += child->implicitHeight();
			totalImplicitHeight += mSpacing;
		}
	}

	totalImplicitHeight -= mSpacing;

	setImplicitWidth(maxImplicitWidth);
	setImplicitHeight(totalImplicitHeight);
}

void CraftColumn::componentComplete()
{
	positionItems();
	QQuickItem::componentComplete();
}

void CraftColumn::onChildImplicitWidthChanged()
{
	positionItems();
}

void CraftColumn::onChildImplicitHeightChanged()
{
	positionItems();
}

void CraftColumn::onChildHeightChanged()
{
	positionItems();
}

void CraftColumn::itemChange(ItemChange change, const ItemChangeData& value)
{
	if (change == ItemChildAddedChange)
	{
		QObject::connect(value.item, &QQuickItem::implicitWidthChanged, this, &CraftColumn::onChildImplicitWidthChanged);
		QObject::connect(value.item, &QQuickItem::implicitHeightChanged, this, &CraftColumn::onChildImplicitHeightChanged);

		//QObject::connect(value.item, &QQuickItem::heightChanged, this, &CraftColumn::onChildHeightChanged);
	}
	else
	{
		positionItems();
	}
	QQuickItem::itemChange(change, value);
}

#pragma once

#include <QQuickItem>

class CraftColumn : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)

public:
	explicit CraftColumn(QQuickItem* parent = nullptr);
	~CraftColumn();

	qreal spacing() const;
	void setSpacing(qreal r);

	void positionItems();
	void componentComplete() override;
	void itemChange(ItemChange, const ItemChangeData&) override;

Q_SIGNALS:
	void spacingChanged();

public /*slots*/:
	void onChildImplicitWidthChanged();
	void onChildImplicitHeightChanged();

	void onChildHeightChanged();

private:
	qreal mSpacing;
};

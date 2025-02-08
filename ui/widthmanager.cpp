#include "widthmanager.h"
#include <QVariant>

	WidthManager::WidthManager(QQuickItem* parent) :
		QQuickItem{ parent },
		mMaxLabelWidth{ 0.0 },
		mMaxControlWidth{ 0.0 }
	{
		setFlag(ItemHasContents, true);

		for(int i=0; i<10; i++) // add 10 empty placeholder columns
			mRegisteredColumns.append(QList<const QQuickItem*>());
	}

	qreal WidthManager::maxLabelWidth() const
	{
		return mMaxLabelWidth;
	}

	qreal WidthManager::maxControlWidth() const
	{
		return mMaxControlWidth;
	}

	QList<qreal> WidthManager::maxColumnWidth() const
	{
		return mMaxColumnWidth;
	}

	void WidthManager::setMaxLabelWidth(const qreal width)
	{
		if (mMaxLabelWidth != width)
		{
			mMaxLabelWidth = width;
			Q_EMIT maxLabelWidthChanged();
		}
	}

	void WidthManager::setMaxControlWidth(const qreal width)
	{
		if (mMaxControlWidth != width)
		{
			mMaxControlWidth = width;
			Q_EMIT maxControlWidthChanged();
		}
	}

	void WidthManager::setMaxColumnWidth(const QList<qreal> width)
	{
		if (mMaxColumnWidth != width)
		{
			mMaxColumnWidth = width;
			Q_EMIT maxColumnWidthChanged();
		}
	}

	Q_INVOKABLE void WidthManager::registerLabel(QVariant v)
	{
		QQuickItem* item = v.value<QQuickItem*>();

		if (item && !mRegisteredLabels.contains(item))
		{
			QObject::connect(item, &QQuickItem::implicitWidthChanged, this, &WidthManager::onLabelImplicitWidthChanged);
			QObject::connect(item, &QQuickItem::implicitHeightChanged, this, &WidthManager::onLabelImplicitHeightChanged);
			QObject::connect(item, &QQuickItem::destroyed, this, &WidthManager::onItemDestroyed);

			mRegisteredLabels.append(item);
			setMaxLabelWidth(calculateMaxLabelWidth());
		}
	}

	Q_INVOKABLE void WidthManager::registerControl(QVariant v)
	{
		QQuickItem* item = v.value<QQuickItem*>();

		if (item && !mRegisteredControls.contains(item))
		{
			QObject::connect(item, &QQuickItem::implicitWidthChanged, this, &WidthManager::onControlImplicitWidthChanged);
			QObject::connect(item, &QQuickItem::implicitHeightChanged, this, &WidthManager::onControlImplicitHeightChanged);
			QObject::connect(item, &QQuickItem::destroyed, this, &WidthManager::onItemDestroyed);

			mRegisteredControls.append(item);
			setMaxControlWidth(calculateMaxControlWidth());
		}
	}

	Q_INVOKABLE void WidthManager::unRegisterControl(QVariant v)
	{
		QQuickItem* item = v.value<QQuickItem*>();

		if (mRegisteredControls.removeOne(item))
			setMaxControlWidth(calculateMaxControlWidth());
	}

	Q_INVOKABLE void WidthManager::registerItemInColumn(QVariant v, int col)
	{
		QQuickItem* item = v.value<QQuickItem*>();

		if (item && !mRegisteredColumns.at(col).contains(item))
		{
			QObject::connect(item, &QQuickItem::implicitWidthChanged, this, &WidthManager::onItemImplicitWidthChanged);
			QObject::connect(item, &QQuickItem::implicitHeightChanged, this, &WidthManager::onItemImplicitHeightChanged);
			QObject::connect(item, &QQuickItem::destroyed, this, &WidthManager::onItemDestroyed);

			mRegisteredColumns[col].append(item);
			setMaxColumnWidth(calculateMaxColumnWidth());
		}
	}

	qreal WidthManager::calculateMaxLabelWidth()
	{
		qreal maxLabelWidth = 0.0;

		for (auto&& item : mRegisteredLabels)
		{
			if (item->implicitWidth() > maxLabelWidth)
				maxLabelWidth = item->implicitWidth();
		}

		return maxLabelWidth;
	}

	qreal WidthManager::calculateMaxControlWidth()
	{
		qreal maxControlWidth = 0.0;

		for (auto&& item : mRegisteredControls)
		{
			if (item->implicitWidth() > maxControlWidth)
				maxControlWidth = item->implicitWidth();
		}

		return maxControlWidth;
	}

	QList<qreal> WidthManager::calculateMaxColumnWidth()
	{
		QList<qreal> maxColumnWidths;
		
		for (auto&& itemsInColumn : mRegisteredColumns)
		{
			qreal maxColumnWidth = 0.0f;
			for (auto&& item : itemsInColumn)
			{
				if (item->implicitWidth() > maxColumnWidth)
					maxColumnWidth = item->implicitWidth();
			}
			maxColumnWidths.append(maxColumnWidth);
		}

		return maxColumnWidths;
	}

	void WidthManager::componentComplete()
	{
		setMaxLabelWidth(calculateMaxLabelWidth());
		setMaxControlWidth(calculateMaxControlWidth());

		setMaxColumnWidth(calculateMaxColumnWidth());

		QQuickItem::componentComplete();
	}

	void WidthManager::onLabelImplicitWidthChanged()
	{
		setMaxLabelWidth(calculateMaxLabelWidth());
	}

	void WidthManager::onLabelImplicitHeightChanged()
	{
		setMaxLabelWidth(calculateMaxLabelWidth());
	}

	void WidthManager::onControlImplicitWidthChanged()
	{
		setMaxControlWidth(calculateMaxControlWidth());
	}

	void WidthManager::onControlImplicitHeightChanged()
	{
		setMaxControlWidth(calculateMaxControlWidth());
	}

	void WidthManager::onItemImplicitWidthChanged()
	{
		setMaxColumnWidth(calculateMaxColumnWidth());
	}

	void WidthManager::onItemImplicitHeightChanged()
	{
		setMaxColumnWidth(calculateMaxColumnWidth());
	}

	void WidthManager::onItemDestroyed(QObject* object)
	{
		const QQuickItem* const item = static_cast<QQuickItem*>(object);

		mRegisteredLabels.removeOne(item);
		mRegisteredControls.removeOne(item);

		for (auto&& itemsInColumn : mRegisteredColumns)
			itemsInColumn.removeOne(item);

		setMaxLabelWidth(calculateMaxLabelWidth());
		setMaxControlWidth(calculateMaxControlWidth());

		setMaxColumnWidth(calculateMaxColumnWidth());
	}


#pragma once

#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#include <QList>
#include <QQuickItem>

	class WidthManager : public QQuickItem
	{
		Q_OBJECT
		Q_PROPERTY(qreal maxLabelWidth     READ maxLabelWidth      WRITE setMaxLabelWidth      NOTIFY maxLabelWidthChanged)
		Q_PROPERTY(qreal maxControlWidth   READ maxControlWidth    WRITE setMaxControlWidth    NOTIFY maxControlWidthChanged)

		Q_PROPERTY(QList<qreal> maxColumnWidth READ maxColumnWidth WRITE setMaxColumnWidth     NOTIFY maxColumnWidthChanged)

	public:
		explicit WidthManager(QQuickItem *parent = nullptr);
		virtual ~WidthManager() override = default;

		qreal maxLabelWidth() const;
		qreal maxControlWidth() const;
		QList<qreal> maxColumnWidth() const;

		void setMaxLabelWidth(const qreal width);
		void setMaxControlWidth(const qreal width);
		void setMaxColumnWidth(const QList<qreal> width);

		Q_INVOKABLE void registerLabel(QVariant v);
		Q_INVOKABLE void registerControl(QVariant v);
		Q_INVOKABLE void unRegisterControl(QVariant v);

		Q_INVOKABLE void registerItemInColumn(QVariant v, int col);

		qreal calculateMaxLabelWidth();
		qreal calculateMaxControlWidth();

		QList<qreal> calculateMaxColumnWidth();

		void componentComplete() override;

	Q_SIGNALS:
		void maxLabelWidthChanged();
		void maxControlWidthChanged();

		void maxColumnWidthChanged();

	public /*slots*/:
		void onLabelImplicitWidthChanged();
		void onLabelImplicitHeightChanged();

		void onControlImplicitWidthChanged();
		void onControlImplicitHeightChanged();

		void onItemImplicitWidthChanged();
		void onItemImplicitHeightChanged();
		
		void onItemDestroyed(QObject* object);

	private:
		qreal mMaxLabelWidth;
		qreal mMaxControlWidth;

		QList<qreal> mMaxColumnWidth;

		QList<const QQuickItem*> mRegisteredLabels;
		QList<const QQuickItem*> mRegisteredControls;

		QList<QList<const QQuickItem*>> mRegisteredColumns;
	};


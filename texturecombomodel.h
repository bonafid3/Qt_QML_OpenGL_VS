#pragma once

#include <QAbstractListModel>
#include <QVector>

struct TextureChoice {
	QString name;
	int data;
};

class ComboBoxModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        TextRole = Qt::UserRole + 1,
        DataRole = Qt::UserRole + 2
    };

    explicit ComboBoxModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setCurrentIndex(int index);

    [[nodiscard]] int currentData() const;

Q_SIGNALS:
        void currentIndexChanged();

private:
    QVector<TextureChoice> mData;

    int mCurrentIndex = 0;
};

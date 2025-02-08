#include "texturecombomodel.h"

ComboBoxModel::ComboBoxModel(QObject* parent) : QAbstractListModel(parent) {
    mData.push_back({ "NO TEXTURE", 0 });
    mData.push_back({ "GEOMETRY", 1 });
    mData.push_back({ "NORMAL", 2 });
    mData.push_back({ "ALBEDO", 3 });
    mData.push_back({ "SHADOW MAP", 4 });
    mData.push_back({ "SSAO", 5 });
    mData.push_back({ "SSAO BLUR", 6 });
}

int ComboBoxModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return mData.size();
}

QVariant ComboBoxModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= mData.size())
        return QVariant();

    switch (role) {
        case TextRole:
            return mData.at(index.row()).name;
            break;
        case DataRole:
			return mData.at(index.row()).data;
			break;
        default:
			//qDebug() << "Unhandled role:" << role << "in file:" << __FILE__ << "function:" << __FUNCTION__;
            break;
    }
    return QVariant{};
}

QHash<int, QByteArray> ComboBoxModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[TextRole] = "textRole";
    return roles;
}

Q_INVOKABLE void ComboBoxModel::setCurrentIndex(int index)
{
    if(mCurrentIndex == index)
        return;
    mCurrentIndex = index;
    Q_EMIT currentIndexChanged();
}

int ComboBoxModel::currentData() const
{
    return mData.at(mCurrentIndex).data;
}

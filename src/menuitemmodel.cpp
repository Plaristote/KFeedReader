#include "menuitemmodel.h"

static MenuItem *fromIndex(const QModelIndex &index)
{
    if (index.isValid() && index.internalPointer())
        return reinterpret_cast<MenuItem *>(index.internalPointer());
    return nullptr;
}

MenuItemModel::MenuItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant MenuItemModel::data(const QModelIndex &index, int role) const
{
    MenuItem *item = fromIndex(index);

    if (item) {
        switch (static_cast<Role>(role)) {
        case DisplayNameRole:
            return item->name();
        case DescriptionRole:
            return item->description();
        case IconUrlRole:
            return item->faviconUrl();
        case MenuItemRole:
            return QVariant::fromValue(reinterpret_cast<QObject *>(item));
        }
    }
    return QVariant();
}

QModelIndex MenuItemModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    MenuItem *parent = fromIndex(parentIndex);
    MenuItem *item;

    if (!parent)
        parent = m_root;
    if (parent) {
        item = parent->childAt(row);
        return createIndex(row, column, item);
    }
    return QModelIndex();
}

QModelIndex MenuItemModel::parent(const QModelIndex &index) const
{
    MenuItem *item = fromIndex(index);
    MenuItem *parent;

    if (item) {
        parent = item->parentItem();
        if (parent)
            return createIndex(parent->row(), 0, parent);
    }
    return QModelIndex();
}

int MenuItemModel::rowCount(const QModelIndex &index) const
{
    MenuItem *item = fromIndex(index);

    if (item)
        return item->childCount();
    return m_root ? m_root->childCount() : 0;
}

int MenuItemModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QHash<int, QByteArray> MenuItemModel::roleNames() const
{
    return {{DisplayNameRole, "displayName"}, {DescriptionRole, "description"}, {IconUrlRole, "iconUrl"}, {MenuItemRole, "menuItem"}};
}

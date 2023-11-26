#ifndef MENUITEMMODEL_H
#define MENUITEMMODEL_H

#include "menuitem.h"
#include <QAbstractItemModel>

class MenuItemModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(MenuItem *root MEMBER m_root NOTIFY rootChanged)
public:
    enum Role { DisplayNameRole = Qt::DisplayRole + 1, DescriptionRole, IconUrlRole, MenuItemRole };

    MenuItemModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &item) const override;
    int rowCount(const QModelIndex &item) const override;
    int columnCount(const QModelIndex &) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void rootChanged();

private:
    MenuItem *m_root = nullptr;
};

#endif

#include "menuitem.h"
#include <QJsonObject>

MenuItem *MenuItem::parentItem() const
{
    return m_parentItem;
}

int MenuItem::row() const
{
    if (m_parentItem)
        return m_parentItem->indexOf(this);
    return 0;
}

void MenuItem::setParentItem(MenuItem *parentItem)
{
    setParent(parentItem);
    m_parentItem = parentItem;
    Q_EMIT parentChanged();
}

void MenuItem::remove()
{
    Q_EMIT removed(this);
    deleteLater();
}

void MenuItem::setName(const QString &_name)
{
    if (m_name == _name) {
        return;
    }

    m_name = _name;
    Q_EMIT nameChanged(m_name);
}

void MenuItem::setDescription(const QString &description)
{
    if (m_description == description) {
        return;
    }

    m_description = description;
    Q_EMIT descriptionChanged(m_description);
}

void MenuItem::loadFromJson(QJsonObject &root)
{
    setName(root.value(QStringLiteral("name")).toString());
    setDescription(root.value(QStringLiteral("description")).toString());
}

void MenuItem::saveToJson(QJsonObject &root) const
{
    root.insert(QStringLiteral("name"), m_name);
    root.insert(QStringLiteral("description"), m_description);
    root.insert(QStringLiteral("type"), static_cast<int>(itemType()));
}

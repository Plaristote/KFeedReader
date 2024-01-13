#include "menuitem.h"
#include <QJsonObject>

MenuItem::MenuItem(QObject *parent)
    : QObject(parent)
{
}

MenuItem::MenuItem(MenuItem &parent)
    : QObject(&parent)
    , m_parentItem(&parent)
{
}

MenuItem *MenuItem::parentItem() const
{
    return m_parentItem;
}

QUrl MenuItem::faviconUrl() const
{
    return QUrl();
}

const QString &MenuItem::name() const
{
    return m_name;
}

const QString &MenuItem::description() const
{
    return m_description;
}

qint64 MenuItem::unreadCount() const
{
    return 0;
}

bool MenuItem::fetching() const
{
    return false;
}

double MenuItem::progress() const
{
    return 0;
}

MenuItem::ItemType MenuItem::itemType() const
{
    return NoItemType;
}

QString MenuItem::view() const
{
    return QString();
}

int MenuItem::indexOf(const QObject *) const
{
    return -1;
}

int MenuItem::childCount() const
{
    return 0;
}

MenuItem *MenuItem::childAt(int) const
{
    return nullptr;
}

int MenuItem::row() const
{
    if (m_parentItem)
        return m_parentItem->indexOf(this);
    return 0;
}

void MenuItem::setParentItem(MenuItem *parentItem)
{
    Q_ASSERT(parentItem != this);
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

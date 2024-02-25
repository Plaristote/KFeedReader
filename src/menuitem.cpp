#include "menuitem.h"
#include <QJsonObject>

MenuItem::MenuItem(QObject *parent)
    : TtlSettings(parent)
{
    connect(this, &MenuItem::parentChanged, this, &MenuItem::updateCrumbs);
}

MenuItem::MenuItem(MenuItem &parent)
    : TtlSettings(&parent)
    , m_parentItem(&parent)
{
    connect(this, &MenuItem::parentChanged, this, &MenuItem::updateCrumbs);
    updateCrumbs();
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

int MenuItem::autoUpdateEnabled() const
{
    if (itemType() == FolderMenuItem) {
        bool allEnabled = true;
        bool allDisabled = true;

        for (int i = 0; i < childCount() && (allEnabled || allDisabled); ++i) {
            switch (childAt(i)->autoUpdateEnabled()) {
            case 0:
                allEnabled = false;
                break;
            case 1:
                allEnabled = allDisabled = false;
                break;
            case 2:
                allDisabled = false;
                break;
            }
        }
        return allEnabled ? 2 : (allDisabled ? 0 : 1);
    }
    return TtlSettings::autoUpdateEnabled();
}

void MenuItem::enableAutoUpdate(bool trigger)
{
    qDebug() << "enableAutoUdpate called" << trigger;
    for (int i = 0; i < childCount(); ++i) {
        childAt(i)->enableAutoUpdate(trigger);
    }
    TtlSettings::enableAutoUpdate(trigger);
    Q_EMIT customTtlChanged();
}

void MenuItem::updateCrumbs()
{
    MenuItem *currentItem = this;

    m_crumbs.clear();
    do {
        m_crumbs.append(currentItem);
        currentItem = currentItem->m_parentItem;
    } while (currentItem);
    std::reverse(m_crumbs.begin(), m_crumbs.end());
    for (int i = 0; i < childCount(); ++i)
        childAt(i)->updateCrumbs();
    Q_EMIT crumbsChanged();
}

void MenuItem::loadFromJson(QJsonObject &root)
{
    setName(root.value(QStringLiteral("name")).toString());
    setDescription(root.value(QStringLiteral("description")).toString());
    TtlSettings::loadFromJson(root);
}

void MenuItem::saveToJson(QJsonObject &root) const
{
    root.insert(QStringLiteral("name"), m_name);
    root.insert(QStringLiteral("description"), m_description);
    root.insert(QStringLiteral("type"), static_cast<int>(itemType()));
    TtlSettings::saveToJson(root);
}

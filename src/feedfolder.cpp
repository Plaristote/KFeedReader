#include "feedfolder.h"
#include "feed.h"
#include <QJsonArray>
#include <QJsonObject>

FeedFolder::FeedFolder(QObject *parent)
    : MenuItem(parent)
{
}

FeedFolder::FeedFolder(FeedFolder &parent)
    : MenuItem(parent)
{
}

FeedFolder::~FeedFolder()
{
    m_items.clear();
    Q_EMIT itemsChanged();
}

QString FeedFolder::view() const
{
    switch (m_displayType) {
    case TreeDisplay:
        return QStringLiteral("qrc:/FolderTreePage.qml");
    case ListDisplay:
        return QStringLiteral("qrc:/FolderListPage.qml");
    }
    Q_ASSERT(false);
    return QString();
}

FeedFolder::DisplayType FeedFolder::displayType() const
{
    return m_displayType;
}

bool FeedFolder::expanded() const
{
    return m_expanded;
}

void FeedFolder::setDisplayType(DisplayType value)
{
    if (m_displayType != value) {
        m_displayType = value;
        Q_EMIT viewChanged();
    }
}

void FeedFolder::setExpanded(bool value)
{
    if (m_expanded != value) {
        m_expanded = value;
        Q_EMIT expandedChanged();
    }
}

QUrl FeedFolder::faviconUrl() const
{
    return QUrl(QStringLiteral("qrc:/icons/folder.png"));
}

MenuItem::ItemType FeedFolder::itemType() const
{
    return MenuItem::FolderMenuItem;
}

FeedFolder *FeedFolder::createFromJson(QJsonObject &root, FeedFolder *parent)
{
    FeedFolder *item = new FeedFolder(*parent);

    item->loadFromJson(root);
    return item;
}

void FeedFolder::loadFromJson(QJsonObject &root)
{
    QJsonArray jsonItems = root.value(QStringLiteral("items")).toArray();

    m_displayType = static_cast<DisplayType>(root.value(QStringLiteral("display")).toInt(1));
    m_expanded = root.value(QStringLiteral("expanded")).toBool(false);
    MenuItem::loadFromJson(root);
    for (QJsonValue value : jsonItems) {
        QJsonObject jsonItem = value.toObject();
        ItemType type = static_cast<ItemType>(jsonItem.value(QStringLiteral("type")).toInt(0));

        switch (type) {
        case FolderMenuItem:
            addItem(createFromJson(jsonItem));
            break;
        case FeedMenuItem:
            addItem(Feed::createFromJson(jsonItem));
            break;
        default:
            break;
        }
    }
}

void FeedFolder::saveToJson(QJsonObject &root) const
{
    QJsonArray itemsJson;

    MenuItem::saveToJson(root);
    root.insert(QStringLiteral("display"), static_cast<int>(m_displayType));
    root.insert(QStringLiteral("expanded"), m_expanded);
    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);
        QJsonObject itemJson;

        menuItem->saveToJson(itemJson);
        itemsJson << itemJson;
    }
    root.insert(QStringLiteral("items"), itemsJson);
}

void FeedFolder::connectItem(MenuItem *menuItem)
{
    connect(menuItem, &MenuItem::unreadCountChanged, this, &FeedFolder::unreadCountChanged);
    connect(menuItem, &MenuItem::fetchingChanged, this, &FeedFolder::fetchingChanged);
    connect(menuItem, &MenuItem::progressChanged, this, &FeedFolder::progressChanged);
    connect(menuItem, &MenuItem::removed, this, &FeedFolder::removeItem);
}

void FeedFolder::disconnectItem(MenuItem *menuItem)
{
    disconnect(menuItem, &MenuItem::unreadCountChanged, this, &FeedFolder::unreadCountChanged);
    disconnect(menuItem, &MenuItem::fetchingChanged, this, &FeedFolder::fetchingChanged);
    disconnect(menuItem, &MenuItem::progressChanged, this, &FeedFolder::progressChanged);
    disconnect(menuItem, &MenuItem::removed, this, &FeedFolder::removeItem);
}

void FeedFolder::addItem(QObject *item)
{
    if (m_items.indexOf(item) < 0) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        Q_EMIT menuItem->removed(menuItem);
        menuItem->setParentItem(this);
        connectItem(menuItem);
        m_items << item;
        Q_EMIT itemsChanged();
        Q_EMIT unreadCountChanged();
    }
}

void FeedFolder::addItemAfter(QObject *item, QObject *previousItem)
{
    if (m_items.indexOf(previousItem) >= 0) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        Q_EMIT menuItem->removed(menuItem);
        m_items.removeAll(item);
        for (auto it = m_items.begin(); it != m_items.end(); ++it) {
            if (*it == previousItem) {
                insertItemAt(++it, menuItem);
                break;
            }
        }
    }
}

void FeedFolder::addItemBefore(QObject *item, QObject *previousItem)
{
    if (m_items.indexOf(previousItem) >= 0) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        Q_EMIT menuItem->removed(menuItem);
        m_items.removeAll(item);
        for (auto it = m_items.begin(); it != m_items.end(); ++it) {
            if (*it == previousItem) {
                insertItemAt(it, menuItem);
                break;
            }
        }
    }
}

void FeedFolder::insertItemAt(QList<QObject *>::iterator it, MenuItem *item)
{
    m_items.insert(it, item);
    connectItem(item);
    Q_EMIT itemsChanged();
    Q_EMIT unreadCountChanged();
}

void FeedFolder::removeItem(QObject *item)
{
    MenuItem *menuItem = qobject_cast<MenuItem *>(item);
    auto index = m_items.indexOf(item);

    if (index >= 0) {
        disconnectItem(menuItem);
        m_items.removeAt(index);
        Q_EMIT itemsChanged();
        Q_EMIT unreadCountChanged();
    }
}

int FeedFolder::indexOf(const QObject *item) const
{
    return m_items.indexOf(const_cast<QObject *>(item));
}

int FeedFolder::childCount() const
{
    return m_items.size();
}

MenuItem *FeedFolder::childAt(int index) const
{
    return reinterpret_cast<MenuItem *>(m_items.at(index));
}

qint64 FeedFolder::unreadCount() const
{
    qint64 total = 0;
    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        if (menuItem)
            total += menuItem->unreadCount();
    }
    return total;
}

void FeedFolder::fetch()
{
    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        menuItem->fetch();
    }
}

void FeedFolder::remove()
{
    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        menuItem->remove();
    }
    MenuItem::remove();
}

bool FeedFolder::fetching() const
{
    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        if (menuItem->fetching())
            return true;
    }
    return false;
}

double FeedFolder::progress() const
{
    int fetchingCount = 0;
    double totalProgress = 0;

    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        if (menuItem->fetching()) {
            fetchingCount++;
            totalProgress += menuItem->progress();
        }
    }
    return totalProgress / fetchingCount;
}

void FeedFolder::triggerBeforeSave()
{
    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);

        menuItem->triggerBeforeSave();
        Q_EMIT menuItem->beforeSave();
    }
    Q_EMIT beforeSave();
}

void FeedFolder::markAsRead()
{
    for (int i = 0; i < childCount(); ++i)
        childAt(i)->markAsRead();
}

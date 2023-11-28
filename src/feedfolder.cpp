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
    return QStringLiteral("qrc:/FeedFolder.qml");
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

    for (QObject *item : m_items) {
        MenuItem *menuItem = qobject_cast<MenuItem *>(item);
        QJsonObject itemJson;

        menuItem->saveToJson(itemJson);
        itemsJson << itemJson;
    }
    MenuItem::saveToJson(root);
    root.insert(QStringLiteral("items"), itemsJson);
}

void FeedFolder::addItem(QObject *item)
{
    MenuItem *menuItem = qobject_cast<MenuItem *>(item);

    menuItem->setParentItem(this);
    if (m_items.indexOf(item) < 0) {
        connect(menuItem, &MenuItem::unreadCountChanged, this, &FeedFolder::unreadCountChanged);
        connect(menuItem, &MenuItem::fetchingChanged, this, &FeedFolder::fetchingChanged);
        connect(menuItem, &MenuItem::progressChanged, this, &FeedFolder::progressChanged);
        connect(menuItem, &MenuItem::removed, this, &FeedFolder::removeItem);
        // beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + 1);
        m_items << item;
        // endInsertRows();
        Q_EMIT itemsChanged();
        Q_EMIT unreadCountChanged();
    }
}

void FeedFolder::removeItem(QObject *item)
{
    MenuItem *menuItem = qobject_cast<MenuItem *>(item);
    auto index = m_items.indexOf(item);

    if (index >= 0) {
        disconnect(menuItem, &MenuItem::unreadCountChanged, this, &FeedFolder::unreadCountChanged);
        disconnect(menuItem, &MenuItem::fetchingChanged, this, &FeedFolder::fetchingChanged);
        disconnect(menuItem, &MenuItem::progressChanged, this, &FeedFolder::progressChanged);
        // beginRemoveRows(QModelIndex(), index, index + 1);
        m_items.removeAt(index);
        // endRemoveRows();
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

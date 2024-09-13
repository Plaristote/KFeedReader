#include "menuitem.h"
#include "feedfolder.h"
#include <QJsonObject>
#include <QRegularExpression>

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

bool MenuItem::matchSearch(const QString &search) const
{
    QRegularExpression regex(search, QRegularExpression::PatternOption::CaseInsensitiveOption);

    return regex.isValid() && regex.match(m_name).hasMatch();
}

MenuItem *MenuItem::fromIndex(const QModelIndex &item)
{
    if (item.internalPointer())
        return reinterpret_cast<MenuItem *>(item.internalPointer());
    return nullptr;
}

QVariant MenuItem::data(const QModelIndex &index, int role) const
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
    return {};
}

QModelIndex MenuItem::index(int row, int column, const QModelIndex &parentIndex) const
{
    const MenuItem *parent = fromIndex(parentIndex);
    const MenuItem *child;

    if (parent)
        return parent->index(row, column);
    child = childAt(row);
    if (child)
        return createIndex(row, column, child);
    else
        qDebug() << "No child at row" << row << "???" << childCount();
    return {};
}

QModelIndex MenuItem::parent(const QModelIndex &index) const
{
    const MenuItem *item = fromIndex(index);

    if (item) {
        MenuItem *parentItem = item->parentItem();

        if (parentItem)
            return createIndex(parentItem->row(), 0, parentItem);
    }
    return {};
}

int MenuItem::rowCount(const QModelIndex &index) const
{
    const MenuItem *item = fromIndex(index);

    return item ? item->childCount() : childCount();
}

int MenuItem::columnCount(const QModelIndex &) const
{
    return 1;
}

QHash<int, QByteArray> MenuItem::roleNames() const
{
    return {{DisplayNameRole, "displayName"}, {DescriptionRole, "description"}, {IconUrlRole, "iconUrl"}, {MenuItemRole, "menuItem"}};
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

bool MenuItem::isAncestorOf(const MenuItem *item) const
{
    for (int i = 0; i < childCount(); ++i) {
        MenuItem *child = childAt(i);

        if (child == item || child->isAncestorOf(item))
            return true;
    }
    return false;
}

void MenuItem::reparent(MenuItem *target, MenuItem *subject)
{
    if (target && subject && target != subject && !subject->isAncestorOf(target)) {
        switch (target->itemType()) {
        case MenuItem::FolderMenuItem:
            beginResetModel();
            reinterpret_cast<FeedFolder *>(target)->addItem(subject);
            endResetModel();
            break;
        default:
            appendNextToSibling(subject, target);
            break;
        }
    }
}

void MenuItem::appendNextToSibling(MenuItem *item, MenuItem *sibling)
{
    FeedFolder *folder = reinterpret_cast<FeedFolder *>(sibling->parentItem());

    if (folder) {
        beginResetModel();
        folder->addItemAfter(item, sibling);
        endResetModel();
    }
}

void MenuItem::appendBeforeSibling(MenuItem *item, MenuItem *sibling)
{
    FeedFolder *folder = reinterpret_cast<FeedFolder *>(sibling->parentItem());

    if (folder) {
        beginResetModel();
        folder->addItemBefore(item, sibling);
        endResetModel();
    }
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

void MenuItem::setSkipPreviewSetting(short value)
{
    m_skipPreviewSetting = value;
    Q_EMIT skipPreviewSettingChanged();
}

short MenuItem::skipPreviewSetting() const
{
    if (m_skipPreviewSetting == 0 && parentItem())
        return parentItem()->skipPreviewSetting();
    return m_skipPreviewSetting;
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
    setSkipPreviewSetting(root.value(QStringLiteral("skipPreview")).toInt(0));
    TtlSettings::loadFromJson(root);
}

void MenuItem::saveToJson(QJsonObject &root) const
{
    root.insert(QStringLiteral("name"), m_name);
    root.insert(QStringLiteral("description"), m_description);
    root.insert(QStringLiteral("type"), static_cast<int>(itemType()));
    if (m_skipPreviewSetting)
        root.insert(QStringLiteral("skipPreview"), static_cast<int>(m_skipPreviewSetting));
    TtlSettings::saveToJson(root);
}

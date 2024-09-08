#include "aggregatedfeed.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedfolder.h"
#include <KLocalizedString>
#include <algorithm>
#define MAX_AGGREGATED_ARTICLE_COUNT 100

FeedArticle *findPreviousArticle(const QList<FeedArticle *> &articles, const FeedArticle *article);
FeedArticle *findNextArticle(const QList<FeedArticle *> &articles, const FeedArticle *article);

AggregatedFeed::AggregatedFeed(QObject *parent)
    : MenuItem(parent)
{
    m_listMaxLength = MAX_AGGREGATED_ARTICLE_COUNT;
    connect(this, &AggregatedFeed::feedsChanged, this, &AggregatedFeed::updateArticles);
    connect(this, &AggregatedFeed::onlyUnreadChanged, this, &AggregatedFeed::updateArticles);
    connect(this, &AggregatedFeed::enabledChanged, this, &AggregatedFeed::updateArticles);
}

AggregatedFeed *AggregatedFeed::createFeed(FeedFolder *parent)
{
    AggregatedFeed *feed = new AggregatedFeed(parent);

    feed->setName(i18n("All"));
    feed->addFolder(parent);
    return feed;
}

AggregatedFeed *AggregatedFeed::createUnreadFeed(FeedFolder *parent)
{
    AggregatedFeed *feed = createFeed(parent);

    feed->setName(i18n("Unread"));
    feed->setOnlyUnread(true);
    return feed;
}

QUrl AggregatedFeed::faviconUrl() const
{
    return QUrl(m_onlyUnread ? QStringLiteral("mail-mark-unread-new") : QStringLiteral("mail-message-new-list"));
}

QString AggregatedFeed::view() const
{
    return QStringLiteral("qrc:/AggregatedFeed.qml");
}

MenuItem::ItemType AggregatedFeed::itemType() const
{
    return AggregateMenuItem;
}

const QList<FeedArticle *> &AggregatedFeed::articles() const
{
    return m_articles;
}

FeedArticle *AggregatedFeed::findPreviousArticle(const FeedArticle *article)
{
    FeedArticle *result = ::findPreviousArticle(m_articles, article);

    if (m_articles.indexOf(result) >= m_listMaxLength)
        loadMore();
    return result;
}

FeedArticle *AggregatedFeed::findNextArticle(const FeedArticle *article)
{
    return ::findNextArticle(m_articles, article);
}

void AggregatedFeed::addFeeds(const QList<Feed *> &feeds)
{
    int addedFeeds = 0;

    for (Feed *feed : feeds) {
        if (m_feeds.indexOf(feed) < 0) {
            connect(feed, &Feed::unreadCountChanged, this, &AggregatedFeed::unreadCountChanged);
            connect(feed, &Feed::fetchingChanged, this, &AggregatedFeed::fetchingChanged);
            connect(feed, &Feed::progressChanged, this, &AggregatedFeed::progressChanged);
            connect(feed, &Feed::articlesChanged, this, &AggregatedFeed::updateArticles);
            connect(feed, &Feed::destroyed, this, &AggregatedFeed::removeAggregatedResource);
            m_feeds.append(feed);
            addedFeeds++;
        }
    }
    if (addedFeeds)
        Q_EMIT feedsChanged();
}

void AggregatedFeed::addFeed(Feed *feed)
{
    addFeeds({feed});
}

void AggregatedFeed::addFolder(FeedFolder *folder)
{
    connect(folder, &FeedFolder::itemsChanged, this, &AggregatedFeed::onFolderItemsChanged);
    connect(folder, &MenuItem::removed, this, &AggregatedFeed::removeAggregatedResource);
    onFolderItemsChanged(folder);
}

void AggregatedFeed::onFolderItemsChanged(FeedFolder *folder)
{
    QList<Feed *> feeds;

    for (int i = 0; i < folder->childCount(); ++i) {
        MenuItem *item = folder->childAt(i);

        if (item->itemType() == FeedMenuItem)
            feeds.append(reinterpret_cast<Feed *>(item));
        else if (item->itemType() == FolderMenuItem)
            addFolder(reinterpret_cast<FeedFolder *>(item));
    }
    addFeeds(feeds);
}

void AggregatedFeed::removeAggregatedResource(QObject *object)
{
    static const QMetaObject *feedMetaObject = Feed().metaObject();
    static const QMetaObject *folderMetaObject = FeedFolder().metaObject();

    if (object->metaObject() == feedMetaObject)
        removeFeed(reinterpret_cast<Feed *>(object));
    else if (object->metaObject() == folderMetaObject)
        removeFolder(reinterpret_cast<FeedFolder *>(object));
}

void AggregatedFeed::removeFeed(Feed *feed)
{
    int index = m_feeds.indexOf(feed);

    if (index >= 0) {
        disconnect(feed, &Feed::unreadCountChanged, this, &AggregatedFeed::unreadCountChanged);
        disconnect(feed, &Feed::fetchingChanged, this, &AggregatedFeed::fetchingChanged);
        disconnect(feed, &Feed::progressChanged, this, &AggregatedFeed::progressChanged);
        disconnect(feed, &Feed::articlesChanged, this, &AggregatedFeed::updateArticles);
        m_feeds.removeAt(index);
        Q_EMIT feedsChanged();
    }
}

void AggregatedFeed::removeFolder(FeedFolder *folder)
{
    static const QMetaObject *feedMetaObject = Feed().metaObject();
    static const QMetaObject *folderMetaObject = FeedFolder().metaObject();
    QList<Feed *> feeds;

    disconnect(folder, &FeedFolder::itemsChanged, this, &AggregatedFeed::onFolderItemsChanged);
    disconnect(folder, &MenuItem::removed, this, &AggregatedFeed::removeAggregatedResource);
    for (int i = 0; i < folder->childCount(); ++i) {
        MenuItem *item = folder->childAt(i);

        if (item->metaObject() == feedMetaObject)
            removeFeed(reinterpret_cast<Feed *>(item));
        else if (item->metaObject() == folderMetaObject)
            removeFolder(reinterpret_cast<FeedFolder *>(item));
    }
}

void AggregatedFeed::fetch()
{
    for (Feed *feed : m_feeds)
        feed->fetch();
}

void AggregatedFeed::updateArticles()
{
    qDebug() << ">>> AggregatedFeed updateArticles" << m_enabled << m_feeds.size();
    if (m_enabled) {
        m_articles.clear();
        for (Feed *feed : m_feeds) {
            for (FeedArticle *article : feed->articles()) {
                if (!m_onlyUnread || !article->isRead()) {
                    m_articles.append(article);
                }
            }
        }
        std::sort(m_articles.begin(), m_articles.end(), [](const FeedArticle *a, const FeedArticle *b) -> bool {
            return a->publicationDate() > b->publicationDate();
        });
        Q_EMIT articlesChanged();
    }
}

int AggregatedFeed::indexOf(const QObject *object) const
{
    return m_articles.indexOf(reinterpret_cast<FeedArticle *>(const_cast<QObject *>(object)));
}

int AggregatedFeed::childCount() const
{
    return m_articles.size();
}

qint64 AggregatedFeed::unreadCount() const
{
    qint64 total = 0;

    for (Feed *feed : m_feeds)
        total += feed->unreadCount();
    return total;
}

bool AggregatedFeed::fetching() const
{
    for (Feed *feed : m_feeds) {
        if (feed->fetching())
            return true;
    }
    return false;
}

double AggregatedFeed::progress() const
{
    double total = 0;
    int fetchingCount = 0;

    for (Feed *feed : m_feeds) {
        if (feed->fetching()) {
            fetchingCount++;
            total += feed->progress();
        }
    }
    return fetchingCount ? (total / fetchingCount) : 0;
}

qint64 AggregatedFeed::listMaxLength() const
{
    return m_listMaxLength;
}

void AggregatedFeed::loadMore()
{
    m_listMaxLength += MAX_AGGREGATED_ARTICLE_COUNT;
    Q_EMIT articlesChanged();
}

qint64 aggregatedFeedCount(QQmlListProperty<QObject> *property)
{
    auto *feed = reinterpret_cast<AggregatedFeed *>(property->object);
    auto *list = static_cast<QList<FeedArticle *> *>(property->data);

    return list->size() < feed->listMaxLength() ? list->size() : feed->listMaxLength();
}

QObject *aggregatedFeedAt(QQmlListProperty<QObject> *property, qint64 index)
{
    auto *list = static_cast<QList<FeedArticle *> *>(property->data);

    if (index >= 0 && index < list->size())
        return list->at(index);
    return nullptr;
}

QQmlListProperty<QObject> AggregatedFeed::qmlArticles()
{
    return QQmlListProperty<QObject>(this, &m_articles, &aggregatedFeedCount, &aggregatedFeedAt);
}

bool AggregatedFeed::hasTextInput() const
{
    return false;
}

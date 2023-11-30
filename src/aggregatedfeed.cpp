#include "aggregatedfeed.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedfolder.h"
#include <algorithm>

AggregatedFeed::AggregatedFeed(QObject *parent)
    : MenuItem(parent)
{
    connect(this, &AggregatedFeed::feedsChanged, this, &AggregatedFeed::updateArticles);
}

QString AggregatedFeed::view() const
{
    return QStringLiteral("qrc:/AggregatedFeed.qml");
}

MenuItem::ItemType AggregatedFeed::itemType() const
{
    return FeedMenuItem;
}

const QList<FeedArticle *> &AggregatedFeed::articles() const
{
    return m_articles;
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
    static const QMetaObject *feedMetaObject = Feed().metaObject();
    static const QMetaObject *folderMetaObject = FeedFolder().metaObject();
    QList<Feed *> feeds;

    connect(folder, &MenuItem::removed, this, &AggregatedFeed::removeAggregatedResource);
    for (int i = 0; i < folder->childCount(); ++i) {
        MenuItem *item = folder->childAt(i);

        if (item->metaObject() == feedMetaObject)
            feeds.append(reinterpret_cast<Feed *>(item));
        else if (item->metaObject() == folderMetaObject)
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
    m_articles.clear();
    for (Feed *feed : m_feeds) {
        for (FeedArticle *article : feed->articles()) {
            m_articles.append(article);
        }
    }
    std::sort(m_articles.begin(), m_articles.end(), [](const FeedArticle *a, const FeedArticle *b) -> bool {
        return a->publicationDate() > b->publicationDate();
    });
    Q_EMIT articlesChanged();
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

QQmlListProperty<QObject> AggregatedFeed::qmlArticles()
{
    return QQmlListProperty<QObject>(this, reinterpret_cast<QList<QObject *> *>(&m_articles));
}

bool AggregatedFeed::hasTextInput() const
{
    return false;
}

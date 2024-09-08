#ifndef AGGREGATEDFEED_H
#define AGGREGATEDFEED_H

#include "menuitem.h"
#include <QQmlListProperty>

class Feed;
class FeedFolder;
class FeedArticle;

class AggregatedFeed : public MenuItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> articles READ qmlArticles NOTIFY articlesChanged)
    Q_PROPERTY(bool hasTextInput READ hasTextInput CONSTANT)
    Q_PROPERTY(bool onlyUnread MEMBER m_onlyUnread NOTIFY onlyUnreadChanged)
    Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged)
public:
    AggregatedFeed(QObject *parent = nullptr);

    static AggregatedFeed *createFeed(FeedFolder *);
    static AggregatedFeed *createUnreadFeed(FeedFolder *);

    QUrl faviconUrl() const override;
    qint64 unreadCount() const override;
    bool fetching() const override;
    double progress() const override;
    ItemType itemType() const override;
    QString view() const override;
    int indexOf(const QObject *) const override;
    int childCount() const override;
    void fetch() override;
    const QList<FeedArticle *> &articles() const;
    bool hasTextInput() const;
    qint64 listMaxLength() const;

    void setOnlyUnread(bool value)
    {
        m_onlyUnread = value;
        Q_EMIT onlyUnreadChanged();
    }

    Q_INVOKABLE FeedArticle *findNextArticle(const FeedArticle *);
    Q_INVOKABLE FeedArticle *findPreviousArticle(const FeedArticle *);

public Q_SLOTS:
    void addFolder(FeedFolder *);
    void addFeeds(const QList<Feed *> &);
    void addFeed(Feed *);
    void removeFeed(Feed *);
    void removeFolder(FeedFolder *);
    void removeAggregatedResource(QObject *);
    void updateArticles();
    void loadMore();

Q_SIGNALS:
    void articlesChanged();
    void feedsChanged();
    void onlyUnreadChanged();
    void enabledChanged();

private Q_SLOTS:
    void onFolderItemsChanged(FeedFolder *);

private:
    QQmlListProperty<QObject> qmlArticles();

    QList<Feed *> m_feeds;
    QList<FeedArticle *> m_articles;
    qint64 m_listMaxLength;
    bool m_onlyUnread = false;
    bool m_enabled = false;
};

#endif

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
public:
    AggregatedFeed(QObject *parent = nullptr);

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

public Q_SLOTS:
    void addFolder(FeedFolder *);
    void addFeeds(const QList<Feed *> &);
    void addFeed(Feed *);
    void removeFeed(Feed *);
    void removeFolder(FeedFolder *);
    void removeAggregatedResource(QObject *);
    void updateArticles();

Q_SIGNALS:
    void articlesChanged();
    void feedsChanged();

private:
    QQmlListProperty<QObject> qmlArticles();

    QList<Feed *> m_feeds;
    QList<FeedArticle *> m_articles;
};

#endif

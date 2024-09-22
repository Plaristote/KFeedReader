#pragma once

#include "cloudprovider.h"
#include <QMap>
#include <QString>

class FeedArticle;

class SharingService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList feedNames READ feedNames NOTIFY feedsChanged)
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool inProgress READ isInProgress NOTIFY inProgressChanged)
public:
    SharingService(CloudProvider &parent);

    bool isEnabled() const;
    void share(const QList<unsigned long> &id, FeedArticle *);

    bool isInProgress() const
    {
        return m_inProgress;
    }
    QStringList feedNames() const
    {
        return m_feeds.keys();
    }

public Q_SLOTS:
    void refreshFeeds();
    void fetchFeeds();
    void share(const QStringList &feedNames, FeedArticle *);

Q_SIGNALS:
    void feedsChanged();
    void enabledChanged();
    void shared(FeedArticle *);
    void failedToShare(FeedArticle *);
    void inProgressChanged();

private:
    unsigned long idForFeedName(const QString &) const;

    CloudProvider &m_cloudProvider;
    QMap<QString, unsigned int> m_feeds;
    bool m_inProgress = false;
};

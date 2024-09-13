#pragma once

#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QUrl>

class FeedArticle;

class SharingService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url MEMBER m_url NOTIFY urlChanged)
    Q_PROPERTY(QByteArray authToken MEMBER m_authToken NOTIFY authTokenChanged)
    Q_PROPERTY(QStringList feedNames READ feedNames NOTIFY feedsChanged)
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool inProgress READ isInProgress NOTIFY inProgressChanged)
public:
    SharingService(QObject *parent = nullptr);

    bool isEnabled() const;
    bool isAuthentifiable() const;
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
    void setSettings(const QUrl &url, const QByteArray &token);
    void refreshFeeds();
    void fetchFeeds();
    void share(const QStringList &feedNames, FeedArticle *);

Q_SIGNALS:
    void urlChanged();
    void authTokenChanged();
    void feedsChanged();
    void enabledChanged();
    void shared(FeedArticle *);
    void failedToShare(FeedArticle *);
    void inProgressChanged();

private:
    unsigned long idForFeedName(const QString &) const;

    QUrl m_url;
    QByteArray m_authToken;
    QMap<QString, unsigned int> m_feeds;
    QNetworkAccessManager m_network;
    bool m_inProgress = false;
};

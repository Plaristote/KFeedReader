#ifndef YOUTUBE_FAVICON_H
#define YOUTUBE_FAVICON_H

#include "../feed.h"

class QNetworkReply;

class FeedYoutubeFavicon : public QObject
{
public:
    FeedYoutubeFavicon(Feed &feed)
        : m_feed(feed)
    {
    }

    static bool isYouTubeFeed(const QUrl &);
    void fetch();
    void fetchFor(const QString &channelId);

private Q_SLOTS:
    void onResponseReceived(QNetworkReply *reply);

private:
    Feed &m_feed;
};

#endif

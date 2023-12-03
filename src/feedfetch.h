#ifndef FEEDFETCH_H
#define FEEDFETCH_H

#include <qobject.h>

class Feed;
class QNetworkReply;

class FeedFetcher : public QObject
{
    Q_OBJECT
public:
    FeedFetcher(Feed &feed);

    void fetch();
    void redirectTo(const QUrl &);
    Feed &feed();

private:
    void readResponse(QNetworkReply *reply);
    void onStarted();
    void onFinished();

    Feed &m_feed;
    int m_requestCounter = 0;
};

#endif

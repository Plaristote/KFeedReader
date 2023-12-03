#ifndef FEEDFAVICON_H
#define FEEDFAVICON_H

#include <qobject.h>

class Feed;

class FeedFavicon : public QObject
{
    Q_OBJECT
public:
    FeedFavicon(Feed &);

    QUrl url() const;
    void fetchFromHtmlPage(const QUrl &);
    void fetch(const QUrl &);

private:
    QString storagePath() const;

    Feed &m_feed;
    unsigned char m_fetchCount = 0;
};

#endif

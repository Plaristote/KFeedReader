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
    QString storagePath() const;
    void fetchFromHtmlPage(const QUrl &);
    void fetch(const QUrl &);

private:
    Feed &m_feed;
    unsigned char m_fetchCount = 0;
};

#endif

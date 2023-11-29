#ifndef FEEDFORMATREADER_H
#define FEEDFORMATREADER_H

#include <QByteArray>

class Feed;

class FeedFormatReader
{
public:
    FeedFormatReader(Feed &feed)
        : feed(feed)
    {
    }

    virtual ~FeedFormatReader()
    {
    }

    virtual void loadBytes(const QByteArray &) = 0;

protected:
    Feed &feed;
};

#endif

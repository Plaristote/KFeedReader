#ifndef FEEDFORMATREADER_H
#define FEEDFORMATREADER_H

class Feed;
class QDomNode;
class QDomElement;

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

    virtual void loadDocument(const QDomNode &) = 0;

protected:
    Feed &feed;
};

#endif

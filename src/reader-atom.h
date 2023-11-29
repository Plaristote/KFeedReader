#ifndef READERATOM_H
#define READERATOM_H

#include "feedformatreader.h"

class FeedArticle;
class QDomNode;
class QDomElement;

class AtomFeedReader : public FeedFormatReader
{
public:
    AtomFeedReader(Feed &feed)
        : FeedFormatReader(feed)
    {
    }

    void loadBytes(const QByteArray &) override;
    void loadDocument(const QDomNode &);
    void loadArticles(const QDomNode &);
    void loadArticle(const QDomElement &, FeedArticle &);
};

#endif

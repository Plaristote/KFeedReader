#ifndef READERATOM_H
#define READERATOM_H

#include "feedformatreader.h"

class FeedArticle;

class AtomFeedReader : public FeedFormatReader
{
public:
    AtomFeedReader(Feed &feed)
        : FeedFormatReader(feed)
    {
    }

    void loadDocument(const QDomNode &) override;
    void loadArticles(const QDomNode &);
    void loadArticle(const QDomElement &, FeedArticle &);
};

#endif

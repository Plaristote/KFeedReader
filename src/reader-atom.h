#ifndef READERATOM_H
#define READERATOM_H

#include "feedformatreader.h"

class AtomFeedReader : public FeedFormatReader
{
public:
    AtomFeedReader(Feed &feed)
        : FeedFormatReader(feed)
    {
    }

    void loadDocument(const QDomNode &) override;
    void loadArticles(const QDomNode &);
};

#endif

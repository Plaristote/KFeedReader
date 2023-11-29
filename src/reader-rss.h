#ifndef READERRSS_H
#define READERRSS_H

#include "feedformatreader.h"

class FeedArticle;

class RssFeedReader : public FeedFormatReader
{
public:
    RssFeedReader(Feed &feed)
        : FeedFormatReader(feed)
    {
    }

    void loadDocument(const QDomNode &) override;
    void loadArticles(const QDomNode &);
    void loadArticle(const QDomElement &, FeedArticle &);
    void loadSkipDays(const QDomElement &);
    void loadSkipHours(const QDomElement &);
    void loadTextInput(const QDomElement &);
};

#endif

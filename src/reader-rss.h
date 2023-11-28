#ifndef READERRSS_H
#define READERRSS_H

#include "feedformatreader.h"

class RssFeedReader : public FeedFormatReader
{
public:
    RssFeedReader(Feed &feed)
        : FeedFormatReader(feed)
    {
    }

    void loadDocument(const QDomNode &) override;
    void loadArticles(const QDomNode &);
    void loadSkipDays(const QDomElement &);
    void loadSkipHours(const QDomElement &);
    void loadTextInput(const QDomElement &);
};

#endif

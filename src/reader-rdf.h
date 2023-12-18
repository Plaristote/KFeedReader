#ifndef READERRDF_H
#define READERRDF_H

#include "feedformatreader.h"

class FeedArticle;
class QDomNode;
class QDomElement;

class RdfFeedReader : public FeedFormatReader
{
public:
    RdfFeedReader(Feed &feed)
        : FeedFormatReader(feed)
    {
    }

    void loadBytes(const QByteArray &) override;
    void loadDocument(const QDomNode &);
    void loadArticles(QDomElement firstItem);
    void loadArticle(const QDomElement &, FeedArticle &);
};

#endif

#ifndef READERJSON_H
#define READERJSON_H

#include "feedformatreader.h"

class FeedArticle;
class QJsonObject;
class QJsonArray;

class JsonFeedReader : public FeedFormatReader
{
public:
    JsonFeedReader(Feed &feed)
        : FeedFormatReader(feed)
    {
    }

    void loadBytes(const QByteArray &) override;
    void loadDocument(const QJsonObject &);
    void loadArticles(const QJsonArray &);
    void loadArticle(const QJsonObject &, FeedArticle &);
    void loadAttachments(const QJsonArray &, FeedArticle &);
};

#endif

#ifndef MEDIALISTUPDATER_H
#define MEDIALISTUPDATER_H

#include "feedarticle.h"

class QDomElement;

template<typename MEDIA_TYPE>
class MediaListUpdater
{
    FeedArticle& article;
    QList<QObject*>& list;
    int index = 0;
    int initialSize;
    bool listChanged = false;

public:
    MediaListUpdater(FeedArticle& article, QList<QObject*>& medias) :
        article(article),
        list(medias),
        initialSize(medias.size())
    {
    }

    ~MediaListUpdater()
    {
        while (index > list.size())
        {
            list.takeLast()->deleteLater();
            listChanged = true;
        }
        if (listChanged)
        {
            Q_EMIT article.mediasChanged();
        }
    }

    void append(const QDomElement& element)
    {
        if (index < initialSize)
        {
            MEDIA_TYPE* media = qobject_cast<MEDIA_TYPE*>(list.at(index));
            media->loadFromXml(element);
        }
        else
        {
            MEDIA_TYPE* media = new MEDIA_TYPE(&article);
            media->loadFromXml(element);
            list << media;
            listChanged = true;
        }
    }
};

#endif

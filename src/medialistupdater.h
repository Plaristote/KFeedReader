#ifndef MEDIALISTUPDATER_H
#define MEDIALISTUPDATER_H

#include "feedarticle.h"
#include <functional>

template<typename MEDIA_TYPE, typename PARAM_TYPE>
class MediaListUpdater
{
    typedef std::function<void(MEDIA_TYPE &, const PARAM_TYPE &)> Callback;
    FeedArticle &article;
    QList<QObject *> &list;
    int index = 0;
    int initialSize;
    bool listChanged = false;
    Callback callback;

public:
    MediaListUpdater(FeedArticle &article, QList<QObject *> &medias, Callback callback)
        : article(article)
        , list(medias)
        , initialSize(medias.size())
        , callback(callback)
    {
    }

    ~MediaListUpdater()
    {
        while (index > list.size()) {
            list.takeLast()->deleteLater();
            listChanged = true;
        }
        if (listChanged) {
            Q_EMIT article.mediasChanged();
        }
    }

    void append(const PARAM_TYPE &element)
    {
        MEDIA_TYPE *media;

        if (index < initialSize) {
            media = qobject_cast<MEDIA_TYPE *>(list.at(index));
        } else {
            media = new MEDIA_TYPE(&article);
            list << media;
            listChanged = true;
        }
        callback(*media, element);
    }
};

template<typename MEDIA_TYPE>
class XmlMediaListUpdater : public MediaListUpdater<MEDIA_TYPE, QDomElement>
{
public:
    XmlMediaListUpdater(FeedArticle &article, QList<QObject *> &medias)
        : MediaListUpdater<MEDIA_TYPE, QDomElement>(article, medias, std::bind(&MEDIA_TYPE::loadFromXml, std::placeholders::_1, std::placeholders::_2))
    {
    }
};

#endif

#include "reader-atom.h"
#include "feed.h"
#include "feedarticle.h"
#include <QDebug>
#include <QDomDocument>

void AtomFeedReader::loadDocument(const QDomNode &document)
{
    QDomElement feed = document.firstChildElement(QStringLiteral("feed"));
    QDomElement titleElement = feed.firstChildElement(QStringLiteral("title"));
    QDomElement linkElement = feed.firstChildElement(QStringLiteral("link"));
    QDomElement updatedElement = feed.firstChildElement(QStringLiteral("updated"));
    QDomElement publishedElement = feed.firstChildElement(QStringLiteral("published"));
    QDomElement imageElement = feed.firstChildElement(QStringLiteral("image"));
    QDomElement logoElement = feed.firstChildElement(QStringLiteral("logo"));
    QString linkAttribute;

    qDebug() << "> Loading ATOM feed";
    this->feed.setName(titleElement.isNull() ? QString() : titleElement.text());
    while (!linkElement.isNull() && linkElement.attribute(QStringLiteral("rel")) != QStringLiteral("alternate"))
        linkElement = linkElement.nextSiblingElement(QStringLiteral("link"));
    linkAttribute = linkElement.attribute(QStringLiteral("href"));
    qDebug() << "Link element = " << linkElement.attribute(QStringLiteral("href"));
    this->feed.setLink(linkAttribute.isNull() ? QUrl() : QUrl(linkAttribute));
    this->feed.setLastBuildDate(updatedElement.isNull() ? QDateTime::currentDateTime() : QDateTime::fromString(updatedElement.text()));
    this->feed.setPublicationDate(publishedElement.isNull() ? QDateTime() : QDateTime::fromString(publishedElement.text(), Qt::ISODate));
    loadArticles(feed);
    if (imageElement.isNull() && !logoElement.isNull())
        Q_EMIT this->feed.requestFaviconUpdate(QUrl(logoElement.text()));
    else if (!imageElement.isNull())
        Q_EMIT this->feed.requestFaviconUpdate(QUrl(imageElement.text()));
    else
        Q_EMIT this->feed.requestFaviconUpdate(QUrl());
}

void AtomFeedReader::loadArticles(const QDomNode &root)
{
    for (QDomElement itemElement = root.firstChildElement(QStringLiteral("entry")); !itemElement.isNull();
         itemElement = itemElement.nextSiblingElement(QStringLiteral("entry"))) {
        QDomElement idElement = itemElement.firstChildElement(QStringLiteral("id"));
        QDomElement linkElement = itemElement.firstChildElement(QStringLiteral("link"));
        QString guid = idElement.isNull() ? QString() : idElement.text();
        QUrl link = linkElement.isNull() ? QUrl() : QUrl(linkElement.attribute(QStringLiteral("href")));
        FeedArticle *article = !guid.isEmpty() ? feed.findArticleByGuid(guid) : feed.findArticleByLink(link);

        if (article)
            article->loadFromAtom(itemElement);
        else {
            article = feed.newArticle();
            article->loadFromAtom(itemElement);
            feed.insertArticle(article);
        }
    }
    Q_EMIT feed.articlesChanged();
}

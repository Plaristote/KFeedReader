#include "reader-atom.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedarticlemedia.h"
#include "feedfavicon.h"
#include "medialistupdater.h"
#include <QDebug>
#include <QDomDocument>

void AtomFeedReader::loadBytes(const QByteArray &bytes)
{
    QDomDocument document;

    document.setContent(bytes);
    loadDocument(document);
}

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
    if (!logoElement.isNull())
        this->feed.setLogoUrl(QUrl(logoElement.text()));
    if (imageElement.isNull() && !logoElement.isNull())
        this->feed.setFaviconUrl(QUrl(logoElement.text()));
    else if (!imageElement.isNull())
        this->feed.setFaviconUrl(QUrl(imageElement.text()));
    else
        this->feed.loadFaviconFrom(this->feed.link());
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
            loadArticle(itemElement, *article);
        else {
            article = feed.newArticle();
            loadArticle(itemElement, *article);
            feed.insertArticle(article);
        }
    }
    Q_EMIT feed.articlesChanged();
}

void AtomFeedReader::loadArticle(const QDomElement &node, FeedArticle &article)
{
    QDomElement guidElement = node.firstChildElement(QStringLiteral("id"));
    QDomElement titleElement = node.firstChildElement(QStringLiteral("title"));
    QDomElement updatedElement = node.firstChildElement(QStringLiteral("updated"));
    QDomElement publishedElement = node.firstChildElement(QStringLiteral("published"));
    QDomElement linkElement = node.firstChildElement(QStringLiteral("link"));
    QDomElement authorElement = node.firstChildElement(QStringLiteral("author"));
    QDomElement authorNameElement = authorElement.firstChildElement(QStringLiteral("name"));
    QDomElement authorUrlElement = authorElement.firstChildElement(QStringLiteral("uri"));
    QDomElement summaryElement = node.firstChildElement(QStringLiteral("summary"));
    QDomElement mediaGroupElement = node.firstChildElement(QStringLiteral("media:group"));
    XmlMediaListUpdater<FeedArticleMedia> mediaList(article, article.m_medias);

    if (publishedElement.isNull())
        publishedElement = updatedElement;
    article.setAuthor(authorNameElement.isNull() ? QString() : authorNameElement.text());
    article.setAuthorUrl(authorUrlElement.isNull() ? QUrl() : QUrl(authorUrlElement.text()));
    article.setDescription(summaryElement.isNull() ? QString() : summaryElement.text());
    article.setGuid(guidElement.isNull() ? QString() : guidElement.text());
    article.setLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.attribute(QStringLiteral("href"))));
    article.setPublicationDate(publishedElement.isNull() ? QDateTime() : QDateTime::fromString(publishedElement.text(), Qt::ISODate));
    article.setTitle(titleElement.isNull() ? QString() : titleElement.text());

    for (QDomElement mediaGroup = node.firstChildElement(QStringLiteral("media:group")); !mediaGroup.isNull();
         mediaGroup = mediaGroup.nextSiblingElement(QStringLiteral("media:group"))) {
        mediaList.append(mediaGroup);
    }
}

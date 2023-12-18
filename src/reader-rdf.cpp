#include "reader-rdf.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedfavicon.h"
#include <QDomDocument>

QUrl baseUrlForLink(const QUrl &link); // defined in RSS reader

void RdfFeedReader::loadBytes(const QByteArray &bytes)
{
    QDomDocument document;

    document.setContent(bytes);
    loadDocument(document);
}

static QDomElement findElement(QDomElement root, const QStringList &candidates)
{
    for (const auto &candidate : candidates) {
        QDomElement element = root.firstChildElement(candidate);

        if (!element.isNull())
            return element;
    }
    return QDomElement();
}

static QDomElement findElementWithOptionalNamespace(QDomElement root, const QString &tagName, const QString &ns = QStringLiteral("dc:"))
{
    return findElement(root, QStringList() << tagName << ns + tagName);
}

static QString findAttribute(QDomElement element, const QStringList &candidates)
{
    for (const auto &candidate : candidates) {
        if (element.hasAttribute(candidate))
            return element.attribute(candidate);
    }
    return QString();
}

void RdfFeedReader::loadDocument(const QDomNode &document)
{
    QDomElement rdf = document.firstChildElement(QStringLiteral("rdf:RDF"));
    QDomElement channel = rdf.firstChildElement(QStringLiteral("channel"));
    QDomElement titleElement = findElementWithOptionalNamespace(channel, QStringLiteral("title"));
    QDomElement descriptionElement = findElementWithOptionalNamespace(channel, QStringLiteral("description"));
    QDomElement linkElement = findElementWithOptionalNamespace(channel, QStringLiteral("link"));
    QDomElement imageElement = findElementWithOptionalNamespace(channel, QStringLiteral("image"));
    QDomElement languageElement = findElementWithOptionalNamespace(channel, QStringLiteral("language"));
    QDomElement firstArticle = rdf.firstChildElement(QStringLiteral("item"));

    feed.setDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
    feed.setLanguage(languageElement.isNull() ? QString() : languageElement.text());
    feed.setLink(baseUrlForLink(linkElement.isNull() ? feed.xmlUrl() : QUrl(linkElement.text())));
    feed.setName(titleElement.isNull() ? QString() : titleElement.text());
    loadArticles(firstArticle);
    if (imageElement.isNull())
        feed.loadFaviconFrom(feed.link());
    else {
        QString imageUrl = findAttribute(imageElement, QStringList() << QStringLiteral("rdf:about") << QStringLiteral("rdf:resource"));
        QDomElement urlElement = imageElement.firstChildElement(QStringLiteral("url"));

        feed.setFaviconUrl(QUrl(urlElement.isNull() ? imageUrl : urlElement.text()));
        feed.setLogoUrl(QUrl(imageUrl));
    }
}

void RdfFeedReader::loadArticles(QDomElement itemElement)
{
    while (!itemElement.isNull()) {
        QString href = itemElement.attribute(QStringLiteral("rdf:about"));
        FeedArticle *article = feed.findArticleByLink(QUrl(href));

        if (article)
            loadArticle(itemElement, *article);
        else {
            article = feed.newArticle();
            loadArticle(itemElement, *article);
            feed.insertArticle(article);
        }
        itemElement = itemElement.nextSiblingElement(QStringLiteral("item"));
    }
}

void RdfFeedReader::loadArticle(const QDomElement &itemElement, FeedArticle &article)
{
    QDomElement titleElement = findElementWithOptionalNamespace(itemElement, QStringLiteral("title"));
    QDomElement dateElement = findElementWithOptionalNamespace(itemElement, QStringLiteral("date"));
    QDomElement descriptionElement = findElementWithOptionalNamespace(itemElement, QStringLiteral("description"));
    QDomElement contentElement = findElementWithOptionalNamespace(itemElement, QStringLiteral("content"));
    QDomElement encodedContentElement = findElementWithOptionalNamespace(itemElement, QStringLiteral("content:encoded"));

    article.setTitle(titleElement.isNull() ? QString() : titleElement.text());
    article.setPublicationDate(dateElement.isNull() ? QDateTime() : QDateTime::fromString(dateElement.text(), Qt::ISODateWithMs));
    article.setLink(QUrl(itemElement.attribute(QStringLiteral("rdf:about"))));
    article.setDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
}

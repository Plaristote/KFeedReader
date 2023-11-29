#include "reader-rss.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedarticleenclosure.h"
#include <QDebug>
#include <QDomDocument>

void RssFeedReader::loadBytes(const QByteArray &bytes)
{
    QDomDocument document;

    document.setContent(bytes);
    loadDocument(document);
}

void RssFeedReader::loadDocument(const QDomNode &document)
{
    QDomElement rss = document.firstChildElement(QStringLiteral("rss"));
    QDomElement channel = rss.firstChildElement(QStringLiteral("channel"));
    QDomElement categoryElement = channel.firstChildElement(QStringLiteral("category"));
    QDomElement copyrightElement = channel.firstChildElement(QStringLiteral("copyright"));
    QDomElement descriptionElement = channel.firstChildElement(QStringLiteral("description"));
    QDomElement imageElement = channel.firstChildElement(QStringLiteral("image"));
    QDomElement languageElement = channel.firstChildElement(QStringLiteral("language"));
    QDomElement lastBuildDateElement = channel.firstChildElement(QStringLiteral("lastBuildDate"));
    QDomElement linkElement = channel.firstChildElement(QStringLiteral("link"));
    QDomElement managingEditorElement = channel.firstChildElement(QStringLiteral("managingEditor"));
    QDomElement pubDateElement = channel.firstChildElement(QStringLiteral("pubDate"));
    QDomElement skipDaysElement = channel.firstChildElement(QStringLiteral("skipDays"));
    QDomElement skipHoursElement = channel.firstChildElement(QStringLiteral("skipHours"));
    QDomElement textInputElement = channel.firstChildElement(QStringLiteral("textinput"));
    QDomElement titleElement = channel.firstChildElement(QStringLiteral("title"));
    QDomElement webMasterElement = channel.firstChildElement(QStringLiteral("webMaster"));
    QDomElement ttlElement = channel.firstChildElement(QStringLiteral("ttl"));

    qDebug() << "> Loading RSS feed";
    feed.setCategory(categoryElement.isNull() ? QString() : categoryElement.text());
    feed.setCopyright(copyrightElement.isNull() ? QString() : copyrightElement.text());
    feed.setDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
    feed.setLanguage(languageElement.isNull() ? QString() : languageElement.text());
    feed.setLastBuildDate(lastBuildDateElement.isNull() ? QDateTime::currentDateTime() : QDateTime::fromString(lastBuildDateElement.text()));
    feed.setLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.text()));
    feed.setManagingEditor(managingEditorElement.isNull() ? QString() : managingEditorElement.text());
    feed.setPublicationDate(pubDateElement.isNull() ? QDateTime() : QDateTime::fromString(pubDateElement.text(), Qt::RFC2822Date));
    loadSkipDays(skipDaysElement);
    loadSkipHours(skipHoursElement);
    loadTextInput(textInputElement);
    feed.setName(titleElement.isNull() ? QString() : titleElement.text());
    feed.setWebmaster(webMasterElement.isNull() ? QString() : webMasterElement.text());
    feed.m_ttl = ttlElement.isNull() ? 0 : ttlElement.text().toInt();
    loadArticles(channel);
    Q_EMIT feed.requestFaviconUpdate(imageElement.isNull() ? QUrl() : QUrl(imageElement.text()));
}

void RssFeedReader::loadArticles(const QDomNode &root)
{
    for (QDomElement itemElement = root.firstChildElement(QStringLiteral("item")); !itemElement.isNull();
         itemElement = itemElement.nextSiblingElement(QStringLiteral("item"))) {
        QDomElement guidElement = itemElement.firstChildElement(QStringLiteral("guid"));
        QDomElement linkElement = itemElement.firstChildElement(QStringLiteral("link"));
        QString guid = guidElement.isNull() ? QString() : guidElement.text();
        QUrl link = linkElement.isNull() ? QUrl() : QUrl(linkElement.text());
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

void RssFeedReader::loadArticle(const QDomElement &node, FeedArticle &article)
{
    QDomElement authorElement = node.firstChildElement(QStringLiteral("author"));
    QDomElement categoryElement = node.firstChildElement(QStringLiteral("category"));
    QDomElement commentsElement = node.firstChildElement(QStringLiteral("comments"));
    QDomElement descriptionElement = node.firstChildElement(QStringLiteral("description"));
    QDomElement enclosureElement = node.firstChildElement(QStringLiteral("enclosure"));
    QDomElement guidElement = node.firstChildElement(QStringLiteral("guid"));
    QDomElement linkElement = node.firstChildElement(QStringLiteral("link"));
    QDomElement pubDateElement = node.firstChildElement(QStringLiteral("pubDate"));
    QDomElement titleElement = node.firstChildElement(QStringLiteral("title"));

    article.setAuthor(authorElement.isNull() ? QString() : authorElement.text());
    article.setCategory(categoryElement.isNull() ? QString() : categoryElement.text());
    article.setComments(commentsElement.isNull() ? QUrl() : QUrl(commentsElement.text()));
    article.setDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
    article.setGuid(guidElement.isNull() ? QString() : guidElement.text());
    article.setLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.text()));
    article.setPublicationDate(pubDateElement.isNull() ? QDateTime() : QDateTime::fromString(pubDateElement.text(), Qt::RFC2822Date));
    article.setTitle(titleElement.isNull() ? QString() : titleElement.text());
    article.clearMedias();
    if (!enclosureElement.isNull()) {
        auto *enclosure = new FeedArticleEnclosure(&article);

        enclosure->loadFromXml(enclosureElement);
        article.m_medias << enclosure;
    }
    Q_EMIT article.mediasChanged();
}

void RssFeedReader::loadSkipDays(const QDomElement &element)
{
    feed.m_skipDays.clear();
    for (QDomElement child = element.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
        static const QHash<QString, unsigned short> dayMap{{QStringLiteral("Monday"), 1},
                                                           {QStringLiteral("Tuesday"), 2},
                                                           {QStringLiteral("Wednesday"), 3},
                                                           {QStringLiteral("Thursday"), 4},
                                                           {QStringLiteral("Friday"), 5},
                                                           {QStringLiteral("Saturday"), 6},
                                                           {QStringLiteral("Sunday"), 7}};
        auto it = dayMap.constFind(child.text());
        unsigned short day = it != dayMap.end() ? it.value() : 0;

        if (day != 0 && feed.m_skipDays.indexOf(day) == -1)
            feed.m_skipDays << day;
    }
    Q_EMIT feed.skipDaysChanged();
}

void RssFeedReader::loadSkipHours(const QDomElement &element)
{
    feed.m_skipHours.clear();
    for (QDomElement child = element.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
        unsigned short hour = child.text().toUShort();

        if (feed.m_skipHours.indexOf(hour) == -1)
            feed.m_skipHours << hour;
    }
    Q_EMIT feed.skipHoursChanged();
}

void RssFeedReader::loadTextInput(const QDomElement &element)
{
    QDomElement descriptionElement = element.firstChildElement(QStringLiteral("description"));
    QDomElement nameElement = element.firstChildElement(QStringLiteral("name"));
    QDomElement linkElement = element.firstChildElement(QStringLiteral("link"));
    QDomElement titleElement = element.firstChildElement(QStringLiteral("title"));

    feed.setTextInputDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
    feed.setTextInputName(nameElement.isNull() ? QString() : nameElement.text());
    feed.setTextInputLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.text()));
    feed.setTextInputTitle(titleElement.isNull() ? QString() : titleElement.text());
}

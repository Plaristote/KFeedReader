#include "reader-json.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedarticleenclosure.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

enum FeedAttribute {
    TitleAttribute,
    LinkAttribute,
    DescriptionAttribute,
    AuthorAttribute,
    ArticlesAttribute,
    IconAttribute,
    IdAttribute,
    ArticleLinkAttribute,
    HtmlDescriptionAttribute,
    TextDescriptionAttribute,
    ArticleDescriptionAttribute,
    PublicationDateAttribute,
    ModifiedDateAttribute,
    NameAttribute,
    UrlAttribute,
    ExternalLinkAttribute,
    AttachmentsAttribute,
    SizeAttribute,
    MimetypeAttribute
};

static const QMap<FeedAttribute, QString> attributes{{TitleAttribute, QStringLiteral("title")},
                                                     {LinkAttribute, QStringLiteral("home_page_url")},
                                                     {DescriptionAttribute, QStringLiteral("description")},
                                                     {AuthorAttribute, QStringLiteral("author")},
                                                     {ArticlesAttribute, QStringLiteral("items")},
                                                     {IconAttribute, QStringLiteral("icon")},
                                                     {IdAttribute, QStringLiteral("id")},
                                                     {ArticleLinkAttribute, QStringLiteral("url")},
                                                     {HtmlDescriptionAttribute, QStringLiteral("content_html")},
                                                     {ArticleDescriptionAttribute, QStringLiteral("summary")},
                                                     {PublicationDateAttribute, QStringLiteral("date_published")},
                                                     {ModifiedDateAttribute, QStringLiteral("date_modified")},
                                                     {NameAttribute, QStringLiteral("name")},
                                                     {UrlAttribute, QStringLiteral("url")},
                                                     {AttachmentsAttribute, QStringLiteral("attachments")},
                                                     {SizeAttribute, QStringLiteral("size_in_bytes")},
                                                     {MimetypeAttribute, QStringLiteral("mime_type")}};

static QDateTime parseJsonDate(const QJsonValue &value, const QDateTime &defaultValue)
{
    if (value.isString())
        return QDateTime::fromString(value.toString(), QStringLiteral("yyyy-MM-ddTHH:mm:ss"));
    return defaultValue;
}

void JsonFeedReader::loadBytes(const QByteArray &bytes)
{
    QJsonDocument document = QJsonDocument::fromJson(bytes);

    loadDocument(document.object());
}

void JsonFeedReader::loadDocument(const QJsonObject &document)
{
    QJsonArray articles = document[attributes[ArticlesAttribute]].toArray();

    feed.setName(document[attributes[TitleAttribute]].toString());
    feed.setLink(QUrl(document[attributes[LinkAttribute]].toString()));
    feed.setDescription(document[attributes[DescriptionAttribute]].toString());
    loadArticles(articles);
}

void JsonFeedReader::loadArticles(const QJsonArray &items)
{
    for (const QJsonValue &itemValue : items) {
        QJsonObject item = itemValue.toObject();
        QString id = item[attributes[IdAttribute]].toString();
        FeedArticle *article = feed.findArticleByGuid(id);

        if (article)
            loadArticle(item, *article);
        else {
            article = feed.newArticle();
            loadArticle(item, *article);
            feed.insertArticle(article);
        }
    }
    Q_EMIT feed.articlesChanged();
}

void JsonFeedReader::loadArticle(const QJsonObject &data, FeedArticle &article)
{
    QJsonValue authorValue = data[attributes[AuthorAttribute]];
    QJsonValue attachmentsValue = data[attributes[AttachmentsAttribute]];
    QJsonValue dateValue = data[attributes[PublicationDateAttribute]];
    QJsonValue modificationDateValue = data[attributes[ModifiedDateAttribute]];
    QDateTime modificationDate = parseJsonDate(modificationDateValue, QDateTime::currentDateTime());

    article.setGuid(data[attributes[IdAttribute]].toString());
    article.setTitle(data[attributes[TitleAttribute]].toString());
    article.setLink(QUrl(data[attributes[ArticleLinkAttribute]].toString()));
    if (data[attributes[HtmlDescriptionAttribute]].isString())
        article.setDescription(data[attributes[HtmlDescriptionAttribute]].toString());
    else if (data[attributes[TextDescriptionAttribute]].isString())
        article.setDescription(data[attributes[TextDescriptionAttribute]].toString());
    else
        article.setDescription(data[attributes[ArticleDescriptionAttribute]].toString());
    if (authorValue.isObject()) {
        QJsonObject author = authorValue.toObject();

        article.setAuthor(author[attributes[NameAttribute]].toString());
        article.setAuthorUrl(QUrl(author[attributes[UrlAttribute]].toString()));
    }
    article.setPublicationDate(parseJsonDate(dateValue, modificationDate));
    article.setSource(QUrl(data[attributes[ExternalLinkAttribute]].toString()));
    if (data[attributes[AttachmentsAttribute]].isArray())
        loadAttachments(data[attributes[AttachmentsAttribute]].toArray(), article);
}

void JsonFeedReader::loadAttachments(const QJsonArray &list, FeedArticle &article)
{
    article.clearMedias();
    for (const QJsonValue &value : list) {
        FeedArticleEnclosure *media = new FeedArticleEnclosure(&article);
        QJsonObject attachment = value.toObject();

        media->setTitle(attachment[attributes[TitleAttribute]].toString());
        media->setUrl(QUrl(attachment[attributes[UrlAttribute]].toString()));
        media->setSize(attachment[attributes[SizeAttribute]].toInt());
        media->setType(attachment[attributes[MimetypeAttribute]].toString());
        article.m_medias.append(media);
    }
    Q_EMIT article.mediasChanged();
}

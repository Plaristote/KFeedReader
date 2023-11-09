#include "feedarticle.h"
#include "feedarticleenclosure.h"
#include "feedarticlemedia.h"
#include <QDomElement>
#include <QJsonArray>
#include <QJsonObject>

FeedArticle::FeedArticle(QObject *parent)
    : QObject(parent)
{
}

FeedArticle::~FeedArticle() noexcept
{
    clearMedias();
}

void FeedArticle::clearMedias()
{
    for (QObject *media : m_medias)
        media->deleteLater();
    m_medias.clear();
}

void FeedArticle::loadFromJson(QJsonObject &root)
{
    setRead(root.value(QStringLiteral("read")).toBool());
    setAuthor(root.value(QStringLiteral("author")).toString());
    setAuthorUrl(QUrl(root.value(QStringLiteral("authorUrl")).toString()));
    setCategory(root.value(QStringLiteral("category")).toString());
    setComments(QUrl(root.value(QStringLiteral("comments")).toString()));
    setDescription(root.value(QStringLiteral("description")).toString());
    setGuid(root.value(QStringLiteral("guid")).toString());
    setLink(QUrl(root.value(QStringLiteral("link")).toString()));
    setPublicationDate(QDateTime::fromString(root.value(QStringLiteral("pubDate")).toString(), Qt::ISODate));
    setSource(QUrl(root.value(QStringLiteral("source")).toString()));
    setTitle(root.value(QStringLiteral("title")).toString());
}

void FeedArticle::saveToJson(QJsonObject &root)
{
    root.insert(QStringLiteral("read"), isRead());
    root.insert(QStringLiteral("author"), author());
    root.insert(QStringLiteral("authorUrl"), authorUrl().toString());
    root.insert(QStringLiteral("category"), category());
    root.insert(QStringLiteral("comments"), comments().toString());
    root.insert(QStringLiteral("description"), description());
    root.insert(QStringLiteral("guid"), guid());
    root.insert(QStringLiteral("link"), link().toString());
    root.insert(QStringLiteral("pubDate"), publicationDate().toString(Qt::ISODate));
    root.insert(QStringLiteral("source"), source().toString());
    root.insert(QStringLiteral("title"), title());
}

void FeedArticle::loadFromAtom(const QDomElement &node)
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

    setAuthor(authorNameElement.isNull() ? QString() : authorNameElement.text());
    setAuthorUrl(authorUrlElement.isNull() ? QUrl() : QUrl(authorUrlElement.text()));
    setDescription(summaryElement.isNull() ? QString() : summaryElement.text());
    setGuid(guidElement.isNull() ? QString() : guidElement.text());
    setLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.attribute(QStringLiteral("href"))));
    setPublicationDate(publishedElement.isNull() ? QDateTime() : QDateTime::fromString(publishedElement.text(), Qt::ISODate));
    setTitle(titleElement.isNull() ? QString() : titleElement.text());
    clearMedias();
    for (QDomElement mediaGroup = node.firstChildElement(QStringLiteral("media:group")); !mediaGroup.isNull();
         mediaGroup = mediaGroup.nextSiblingElement(QStringLiteral("media:group"))) {
        auto *media = new FeedArticleMedia(this);

        media->loadFromXml(mediaGroup);
        m_medias << media;
    }
    Q_EMIT mediasChanged();
}

void FeedArticle::loadFromRSS(const QDomElement &node)
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

    setAuthor(authorElement.isNull() ? QString() : authorElement.text());
    setCategory(categoryElement.isNull() ? QString() : categoryElement.text());
    setComments(commentsElement.isNull() ? QUrl() : QUrl(commentsElement.text()));
    setDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
    setGuid(guidElement.isNull() ? QString() : guidElement.text());
    setLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.text()));
    setPublicationDate(pubDateElement.isNull() ? QDateTime() : QDateTime::fromString(pubDateElement.text(), Qt::RFC2822Date));
    setTitle(titleElement.isNull() ? QString() : titleElement.text());
    clearMedias();
    if (!enclosureElement.isNull()) {
        auto *enclosure = new FeedArticleEnclosure(this);

        enclosure->loadFromXml(enclosureElement);
        m_medias << enclosure;
    }
    Q_EMIT mediasChanged();
}

void FeedArticle::setRead(bool value)
{
    if (value != m_read) {
        m_read = value;
        Q_EMIT readChanged(value);
    }
}

QString FeedArticle::author() const
{
    return m_author;
}

void FeedArticle::setAuthor(const QString &author)
{
    if (m_author == author) {
        return;
    }

    m_author = author;
    Q_EMIT authorChanged(m_author);
}

QUrl FeedArticle::authorUrl() const
{
    return m_authorUrl;
}

void FeedArticle::setAuthorUrl(const QUrl &authorUrl)
{
    if (m_authorUrl == authorUrl) {
        return;
    }

    m_authorUrl = authorUrl;
    Q_EMIT authorUrlChanged(m_authorUrl);
}

QString FeedArticle::category() const
{
    return m_category;
}

void FeedArticle::setCategory(const QString &category)
{
    if (m_category == category) {
        return;
    }

    m_category = category;
    Q_EMIT categoryChanged(m_category);
}

QUrl FeedArticle::comments() const
{
    return m_comments;
}

void FeedArticle::setComments(const QUrl &comments)
{
    if (m_comments == comments) {
        return;
    }

    m_comments = comments;
    Q_EMIT commentsChanged(m_comments);
}

QString FeedArticle::description() const
{
    return m_description;
}

void FeedArticle::setDescription(const QString &description)
{
    if (m_description == description) {
        return;
    }

    m_description = description;
    Q_EMIT descriptionChanged(m_description);
}

QString FeedArticle::guid() const
{
    return m_guid;
}

void FeedArticle::setGuid(const QString &guid)
{
    if (m_guid == guid) {
        return;
    }

    m_guid = guid;
    Q_EMIT guidChanged(m_guid);
}

QUrl FeedArticle::link() const
{
    return m_link;
}

void FeedArticle::setLink(const QUrl &link)
{
    if (m_link == link) {
        return;
    }

    m_link = link;
    Q_EMIT linkChanged(m_link);
}

QDateTime FeedArticle::publicationDate() const
{
    return m_publicationDate;
}

void FeedArticle::setPublicationDate(const QDateTime &publicationDate)
{
    if (m_publicationDate == publicationDate) {
        return;
    }

    m_publicationDate = publicationDate;
    Q_EMIT publicationDateChanged(m_publicationDate);
}

QUrl FeedArticle::source() const
{
    return m_source;
}

void FeedArticle::setSource(const QUrl &source)
{
    if (m_source == source) {
        return;
    }

    m_source = source;
    Q_EMIT sourceChanged(m_source);
}

QString FeedArticle::title() const
{
    return m_title;
}

void FeedArticle::setTitle(const QString &title)
{
    if (m_title == title) {
        return;
    }

    m_title = title;
    Q_EMIT titleChanged(m_title);
}

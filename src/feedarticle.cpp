#include "feedarticle.h"
#include "feed.h"
#include "feedarticleenclosure.h"
#include "feedarticlemedia.h"
#include "feedattachment.h"
#include "menuitem.h"
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
    QJsonValue attachments = root.value(QStringLiteral("attachments"));

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
    if (attachments.isArray()) {
        for (QJsonValue attachmentJson : attachments.toArray()) {
            int type = attachmentJson[QStringLiteral("_type")].toInt();
            FeedAttachment *attachment = nullptr;

            switch (type) {
            case FeedAttachment::EnclosureAttachment:
                attachment = new FeedArticleEnclosure(this);
                break;
            case FeedAttachment::MediaAttachment:
                attachment = new FeedArticleMedia(this);
                break;
            }
            if (attachment) {
                attachment->loadFromJson(attachmentJson.toObject());
                m_medias.append(attachment);
            }
        }
    }
}

void FeedArticle::saveToJson(QJsonObject &root)
{
    QJsonArray attachments;

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
    if (m_medias.size() > 0) {
        for (QObject *abstractMedia : m_medias) {
            QJsonObject attachment;

            FeedAttachment *media = reinterpret_cast<FeedAttachment *>(abstractMedia);
            media->saveToJson(attachment);
            attachments << attachment;
        }
        root.insert(QStringLiteral("attachments"), attachments);
    }
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

QUrl FeedArticle::faviconUrl() const
{
    return reinterpret_cast<Feed *>(parent())->faviconUrl();
}

QQmlListProperty<QObject> FeedArticle::qmlCrumbs()
{
    qDebug() << "Feed::qmlCrumbs" << parent();
    return qobject_cast<MenuItem *>(parent())->qmlCrumbs();
}

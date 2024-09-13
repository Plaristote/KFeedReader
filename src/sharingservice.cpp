#include "sharingservice.h"
#include "feedarticle.h"
#include "feedarticleenclosure.h"
#include "feedarticlemedia.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSettings>

static QUrl makeFeedUrl(QUrl url)
{
    url.setPath(QStringLiteral("/share/feeds"));
    return url;
}

static QUrl makeShareUrl(QUrl url)
{
    url.setPath(QStringLiteral("/share"));
    return url;
}

static void decorateRequest(QNetworkRequest &request, const QByteArray &authToken)
{
    request.setRawHeader(QStringLiteral("Accept").toUtf8(), QStringLiteral("application/json").toUtf8());
    request.setRawHeader(QStringLiteral("X-AppToken").toUtf8(), authToken);
}

static QJsonObject enclosureToAttachment(FeedArticleEnclosure *model)
{
    QJsonObject data;

    data[QStringLiteral("title")] = model->title();
    data[QStringLiteral("url")] = model->url().toString();
    data[QStringLiteral("mime_type")] = model->type();
    return data;
}

static QJsonObject mediaToAttachment(FeedArticleMedia *model)
{
    QJsonObject data;

    data[QStringLiteral("title")] = model->title();
    data[QStringLiteral("url")] = model->contentUrl().toString();
    data[QStringLiteral("mime_type")] = QStringLiteral("application/octet-stream");
    return data;
}

static QJsonArray toJsonAttachments(const QList<QObject *> &medias)
{
    QJsonArray attachments;

    for (QObject *abstractMedia : medias) {
        QByteArrayView className = abstractMedia->metaObject()->className();

        if (className == "FeedArticleEnclosure") {
            attachments << enclosureToAttachment(reinterpret_cast<FeedArticleEnclosure *>(abstractMedia));
        } else if (className == "FeedArticleMedia") {
            FeedArticleMedia *media = reinterpret_cast<FeedArticleMedia *>(abstractMedia);

            if (!media->contentUrl().isEmpty()) {
                attachments << mediaToAttachment(media);
            }
        }
    }
    return attachments;
}

static QJsonObject toShareJson(FeedArticle &article, const QList<unsigned long> &feedIds)
{
    QJsonObject data, author, root;
    QJsonArray attachments = toJsonAttachments(article.medias());
    QVariantList feedIdsValue;

    for (unsigned long id : feedIds)
        feedIdsValue << QVariant::fromValue(id);
    data[QStringLiteral("feed_ids")] = QJsonArray::fromVariantList(feedIdsValue);
    data[QStringLiteral("title")] = article.title();
    data[QStringLiteral("article_url")] = article.link().toString();
    data[QStringLiteral("date_published")] = article.publicationDate().toSecsSinceEpoch();
    data[QStringLiteral("content_html")] = article.description();
    if (!article.remoteFaviconUrl().isEmpty())
        data[QStringLiteral("image")] = article.remoteFaviconUrl().toString();
    if (!article.authorUrl().isEmpty())
        author[QStringLiteral("url")] = article.authorUrl().toString();
    if (!article.author().isEmpty())
        author[QStringLiteral("name")] = article.author();
    if (attachments.size() > 0)
        data[QStringLiteral("attachments")] = attachments;
    root[QStringLiteral("article-share")] = data;
    return root;
}

SharingService::SharingService(QObject *parent)
    : QObject(parent)
{
    QSettings settings(QStringLiteral("sharingService"));

    m_url = settings.value("url").toUrl();
    m_authToken = settings.value("token").toByteArray();
    connect(this, &SharingService::feedsChanged, this, &SharingService::enabledChanged);
    connect(this, &SharingService::shared, this, &SharingService::inProgressChanged);
    connect(this, &SharingService::failedToShare, this, &SharingService::inProgressChanged);
    connect(this, &SharingService::urlChanged, this, &SharingService::enabledChanged);
    connect(this, &SharingService::urlChanged, this, &SharingService::refreshFeeds);
    connect(this, &SharingService::authTokenChanged, this, &SharingService::enabledChanged);
    connect(this, &SharingService::authTokenChanged, this, &SharingService::refreshFeeds);
    refreshFeeds();
}

void SharingService::setSettings(const QUrl &url, const QByteArray &token)
{
    QSettings settings(QStringLiteral("sharingService"));

    m_url = url;
    m_authToken = token;
    settings.setValue("url", m_url);
    settings.setValue("token", m_authToken);
    refreshFeeds();
}

bool SharingService::isEnabled() const
{
    qDebug() << "isEnabled called";
    return isAuthentifiable() && m_feeds.size() > 0;
}

bool SharingService::isAuthentifiable() const
{
    return m_authToken.length() && !m_url.isEmpty();
}

unsigned long SharingService::idForFeedName(const QString &feedName) const
{
    auto it = m_feeds.find(feedName);

    return it != m_feeds.end() ? it.value() : 0;
}

void SharingService::share(const QStringList &feedNames, FeedArticle *article)
{
    QList<unsigned long> ids;

    for (const QString &feedName : feedNames) {
        ids << idForFeedName(feedName);
    }
    ids.removeAll(0);
    if (ids.size() > 0) {
        share(ids, article);
    } else {
        qDebug() << "Failed to find ids for shared-feed" << feedNames;
    }
}

void SharingService::share(const QList<unsigned long> &ids, FeedArticle *article)
{
    QNetworkRequest request(makeShareUrl(m_url));
    QNetworkReply *reply;
    QJsonObject articleJson = toShareJson(*article, ids);
    QJsonDocument document(articleJson);
    QByteArray body = document.toJson();

    decorateRequest(request, m_authToken);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json").toUtf8());
    reply = m_network.post(request, body);
    m_inProgress = true;
    connect(reply, &QNetworkReply::finished, this, [this, reply, article]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        m_inProgress = false;
        if (status == 200)
            Q_EMIT shared(article);
        else
            Q_EMIT failedToShare(article);
        reply->deleteLater();
    });
    Q_EMIT inProgressChanged();
}

void SharingService::refreshFeeds()
{
    if (isAuthentifiable()) {
        fetchFeeds();
    }
}

void SharingService::fetchFeeds()
{
    QNetworkRequest request(makeFeedUrl(m_url));
    QNetworkReply *reply;

    decorateRequest(request, m_authToken);
    reply = m_network.get(request);
    m_inProgress = true;
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        m_inProgress = false;
        if (status >= 200 && status < 300) {
            QJsonObject root = QJsonDocument::fromJson(reply->readAll()).object();
            QJsonArray list = root[QStringLiteral("shared-feeds")].toArray();

            m_feeds.clear();
            for (QJsonValue itemValue : list) {
                QJsonObject item = itemValue.toObject();
                QString name = item[QStringLiteral("title")].toString();
                unsigned long id = item[QStringLiteral("id")].toInt();

                m_feeds.insert(name, id);
            }
            Q_EMIT feedsChanged();
        }
        reply->deleteLater();
        Q_EMIT inProgressChanged();
    });
    Q_EMIT inProgressChanged();
}

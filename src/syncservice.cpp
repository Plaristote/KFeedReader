#include "syncservice.h"
#include "feed.h"
#include "feedfolder.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

static const QString lastUpdateKey = QStringLiteral("last_update");
static const QString folderKey = QStringLiteral("feed-tree");
static const QString feedKey = QStringLiteral("feed-article-last");

static QUrl makeFeedUrl(QUrl url, Feed &feed)
{
    url.setPath(QStringLiteral("/feeds/") + feed.uuid());
    return url;
}

static QUrl makeTreeUrl(QUrl url)
{
    url.setPath(QStringLiteral("/feeds"));
    return url;
}

SyncService::SyncService(CloudProvider &parent)
    : QObject(&parent)
    , m_cloudProvider(parent)
{
}

void SyncService::synchronize(Feed &feed)
{
    QUrl url = makeFeedUrl(m_cloudProvider.url(), feed);
    QJsonObject body;
    QJsonArray articlesJson;

    feed.saveArticlesToJson(articlesJson);
    body[lastUpdateKey] = QDateTime::currentSecsSinceEpoch();
    body[feedKey] = articlesJson;
}

static void loadReplyIntoFolder(QNetworkReply &reply, FeedFolder &folder)
{
    QJsonObject data = QJsonDocument::fromJson(reply.readAll()).object();
    QJsonObject feedTree = data[folderKey].toObject();
    QDateTime lastUpdate = QDateTime::fromSecsSinceEpoch(data[lastUpdateKey].toInt());

    // TODO clear article files for removed feeds
    folder.loadFromJson(feedTree);
    folder.setLastModified(lastUpdate);
}

void SyncService::synchronize(FeedFolder &folder)
{
    QNetworkRequest request(makeTreeUrl(m_cloudProvider.url()));
    QNetworkReply *reply;
    QJsonObject body;
    QJsonObject folderJson;
    folder.saveToJson(folderJson);
    body[lastUpdateKey] = folder.lastModified().toSecsSinceEpoch();
    body[folderKey] = folderJson;
    reply = m_cloudProvider.network().post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply, &folder]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        if (status == 200) {
            loadReplyIntoFolder(*reply, folder);
        } else {
            qDebug() << "SyncService: feed-tree already up to date";
        }
        reply->deleteLater();
    });
}

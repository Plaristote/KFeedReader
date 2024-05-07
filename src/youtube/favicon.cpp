#include "./favicon.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

static const QString apiKey = QStringLiteral("AIzaSyDXFNFZsyz8ymc6zYmVv5wIgdEp8K9bIc0");

bool FeedYoutubeFavicon::isYouTubeFeed(const QUrl &url)
{
    QString host = url.host();

    return host.indexOf(QStringLiteral("youtube.com")) >= 0;
}

void FeedYoutubeFavicon::fetch()
{
    QUrlQuery params(m_feed.xmlUrl());
    QString channelId = params.queryItemValue(QStringLiteral("channel_id"));

    fetchFor(channelId);
}

void FeedYoutubeFavicon::fetchFor(const QString &channelId)
{
    QNetworkReply *reply;
    QUrlQuery params;
    QUrl queryUrl(QStringLiteral("https://www.googleapis.com/youtube/v3/channels"));

    params.addQueryItem(QStringLiteral("key"), apiKey);
    params.addQueryItem(QStringLiteral("part"), QStringLiteral("snippet"));
    params.addQueryItem(QStringLiteral("id"), channelId);
    params.addQueryItem(QStringLiteral("fields"), QStringLiteral("items/snippet/thumbnails"));
    queryUrl.setQuery(params);
    reply = m_feed.m_network->get(QNetworkRequest(queryUrl));
    reply->ignoreSslErrors();
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        if (status >= 200 && status < 300)
            onResponseReceived(reply);
        reply->deleteLater();
        deleteLater();
    });
}

void FeedYoutubeFavicon::onResponseReceived(QNetworkReply *reply)
{
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonArray items = document[QStringLiteral("items")].toArray();

    if (items.size() > 0) {
        QJsonObject item = items.first().toObject();
        QJsonObject snippet = item[QStringLiteral("snippet")].toObject();
        QJsonObject thumbnails = snippet[QStringLiteral("thumbnails")].toObject();
        QJsonObject defaultThumbnail = thumbnails[QStringLiteral("default")].toObject();
        QJsonObject mediumThumbnail = thumbnails[QStringLiteral("medium")].toObject();

        m_feed.setFaviconUrl(QUrl(defaultThumbnail[QStringLiteral("url")].toString()));
        m_feed.setLogoUrl(QUrl(mediumThumbnail[QStringLiteral("url")].toString()));
    }
}

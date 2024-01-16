#include "feedfavicon.h"
#include "feed.h"
#include <QFile>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>

static const char *storageFormat = "png";
static const char maxRedirectCount = 5;

void probeHtmlForFavicon(const QByteArray &body, Feed &feed);

FeedFavicon::FeedFavicon(Feed &feed)
    : QObject(&feed)
    , m_feed(feed)
{
}

QUrl FeedFavicon::url() const
{
    QString path = storagePath();

    if (QFile::exists(path))
        return QUrl(QStringLiteral("file:") + path);
    return QUrl(QStringLiteral("qrc:/icons/feed.png"));
}

QString FeedFavicon::storagePath() const
{
    return m_feed.storagePrefix() + QStringLiteral(".") + QString::fromLatin1(storageFormat);
}

void FeedFavicon::fetch(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_feed.m_network->get(request);

    m_fetchCount++;
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        if (status == 302 && m_fetchCount < maxRedirectCount) {
            fetch(QUrl(reply->header(QNetworkRequest::LocationHeader).toString()));
            return;
        } else if (status >= 200 && status < 300) {
            QImage image;

            if (image.loadFromData(reply->readAll())) {
                image.save(storagePath(), storageFormat);
                Q_EMIT m_feed.faviconUrlChanged();
            }
        }
        reply->deleteLater();
        deleteLater();
    });
}

void FeedFavicon::fetchFromHtmlPage(const QUrl &remoteUrl)
{
    QNetworkReply *reply = m_feed.m_network->get(QNetworkRequest(remoteUrl));

    reply->ignoreSslErrors();
    m_fetchCount++;
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        reply->deleteLater();
        if (status > 300 && status < 304 && m_fetchCount < maxRedirectCount) {
            fetchFromHtmlPage(QUrl(reply->header(QNetworkRequest::LocationHeader).toString()));
            return;
        } else if (status >= 200 && status < 300)
            probeHtmlForFavicon(reply->readAll(), m_feed);
        deleteLater();
    });
}

#include "feedfetch.h"
#include "feed.h"
#include "reader-atom.h"
#include "reader-json.h"
#include "reader-rdf.h"
#include "reader-rss.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

void probeHtmlForFeedAndRefetch(const QByteArray &body, FeedFetcher &);

static Feed::FeedType inferFeedType(QNetworkReply &reply, const QByteArray &body)
{
    QString contentType = reply.header(QNetworkRequest::ContentTypeHeader).toString();

    if (contentType.contains(QStringLiteral("atom+xml")))
        return Feed::AtomFeed;
    else if (contentType.contains(QStringLiteral("rss+xml")))
        return Feed::RSSFeed;
    else if (contentType.contains(QStringLiteral("application/rdf+xml")))
        return Feed::RDFFeed;
    else if (contentType.contains(QStringLiteral("application/feed+json")) || contentType.contains(QStringLiteral("application/json")))
        return Feed::JSONFeed;
    else if (contentType.contains(QStringLiteral("text/html")) || contentType.contains(QStringLiteral("application/xhtml+xml")))
        return Feed::HTMLView;
    // Starting from there, we guesstimate:
    else if (body.indexOf(QStringLiteral("<feed").toUtf8()) >= 0)
        return Feed::AtomFeed;
    else if (body.indexOf(QStringLiteral("<rdf:RDF").toUtf8()) >= 0)
        return Feed::RDFFeed;
    return Feed::RSSFeed;
}

FeedFetcher::FeedFetcher(Feed &feed)
    : QObject(&feed)
    , m_feed(feed)
{
}

Feed &FeedFetcher::feed()
{
    return m_feed;
}

void FeedFetcher::fetch()
{
    if (m_requestCounter < 5) {
        QNetworkRequest request(m_feed.m_xmlUrl);
        request.setRawHeader("Accept", "application/rss+xml, application/atom+xml, application/feed+json, text/xml, text/html");
        QNetworkReply *reply = m_feed.m_network->get(request);

        connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesRead, qint64 totalBytes) {
            m_feed.m_progress = static_cast<double>(bytesRead) / totalBytes;
            Q_EMIT m_feed.progressChanged();
        });
        connect(reply, &QNetworkReply::finished, this, std::bind(&FeedFetcher::readResponse, this, reply));
        onStarted();
    } else {
        qDebug() << "/!\\ Won't fetch feed" << m_feed.m_xmlUrl << ": passed max request count";
        onFinished();
    }
}

void FeedFetcher::readResponse(QNetworkReply *reply)
{
    unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

    if (status >= 200 && status < 300) {
        auto body = reply->readAll();

        switch (inferFeedType(*reply, body)) {
        case Feed::RDFFeed:
            RdfFeedReader(m_feed).loadBytes(body);
            break;
        case Feed::RSSFeed:
            RssFeedReader(m_feed).loadBytes(body);
            break;
        case Feed::AtomFeed:
            AtomFeedReader(m_feed).loadBytes(body);
            break;
        case Feed::JSONFeed:
            JsonFeedReader(m_feed).loadBytes(body);
            break;
        case Feed::HTMLView:
            probeHtmlForFeedAndRefetch(body, *this);
            break;
        }
    } else if (status > 300 && status < 304) {
        redirectTo(QUrl(reply->header(QNetworkRequest::LocationHeader).toString()));
        return;
    } else if (status == 406) {
        qDebug() << "/!\\ Fetch status" << status << m_feed.xmlUrl() << ", acceptable formats are:" << reply->readAll();
    } else {
        qDebug() << "/!\\ Fetch status" << status << m_feed.xmlUrl();
    }
    onFinished();
}

void FeedFetcher::redirectTo(const QUrl &url)
{
    m_feed.setXmlUrl(url);
    m_requestCounter++;
    fetch();
}

void FeedFetcher::onStarted()
{
    m_feed.m_progress = 0;
    m_feed.m_fetching = true;
    Q_EMIT m_feed.fetchingChanged();
}

void FeedFetcher::onFinished()
{
    m_feed.setLastUpdate(QDateTime::currentDateTime());
    m_feed.m_progress = 1;
    m_feed.m_fetching = false;
    Q_EMIT m_feed.fetchingChanged();
    deleteLater();
}

#include "feed.h"
#include "feedfetch.h"
#include <QDebug>
#include <QRegularExpression>

static QString extractUrlBaseFrom(const QUrl &url)
{
    QString base = url.scheme() + QStringLiteral("://");

    if (!url.userName().isEmpty())
        base += url.userName() + QStringLiteral(":") + url.password() + QStringLiteral("@");
    base += url.host();

    if (url.port() > 0)
        base += QStringLiteral(":") + QString::number(url.port());
    return base;
}

static QUrl makeUrlFromHref(const QUrl &oldUrl, QString href)
{
    QString oldUrlBase = extractUrlBaseFrom(oldUrl);

    qDebug() << "Found HREF for link:" << href;
    if (href.startsWith(QStringLiteral("http://")) || href.startsWith(QStringLiteral("https://")))
        return QUrl(href);
    if (href.startsWith(QStringLiteral("//")))
        href = oldUrl.scheme() + href;
    else if (href.startsWith(QStringLiteral("/")))
        href = extractUrlBaseFrom(oldUrl) + href;
    else
        href = extractUrlBaseFrom(oldUrl) + QStringLiteral("/") + oldUrl.path() + QStringLiteral("/") + href;
    qDebug() << "New URL:" << href;
    return QUrl(href);
}

static void planReFetch(FeedFetcher &feedFetcher, QUrl newUrl)
{
    Feed *feed = &(feedFetcher.feed());
    QTimer *timer = new QTimer(feed);

    QObject::connect(timer, &QTimer::timeout, feed, [feed, timer, newUrl]() {
        FeedFetcher *fetcher = new FeedFetcher(*feed);
        fetcher->redirectTo(newUrl);
        timer->deleteLater();
    });
    timer->setInterval(300);
    timer->setSingleShot(true);
    timer->start();
}

void probeHtmlForFeedAndRefetch(const QByteArray &body, FeedFetcher &feedFetcher)
{
    static const QStringList formatPatterns = QStringList()
        << QStringLiteral("application/rss+xml") << QStringLiteral("application/atom+xml") << QStringLiteral("application/feed+json")
        << QStringLiteral("text/rss+xml") << QStringLiteral("text/atom+xml") << QStringLiteral("application/xml");
    static const QString pattern = QStringLiteral("<link[^>]*type=\"([a-z/+]+)\"[^>]*>");
    QString subject = QString::fromUtf8(body);
    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match;
    Feed &feed = feedFetcher.feed();
    int offset = 0;

    qDebug() << "Using pattern:" << pattern;
    while ((match = regex.match(subject, offset)).hasMatch()) {
        QString linkType = match.captured(1);
        QString linkElement = match.captured(0);

        qDebug() << "Probed feed link element in HTML view:" << linkElement;
        qDebug() << "Link type: " << linkType;

        offset = match.capturedEnd(0);
        if (!formatPatterns.contains(linkType))
            continue;

        static const QString hrefPattern = QStringLiteral("href=\"([^\"]*)\"");
        QRegularExpression hrefRegex(hrefPattern, QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch hrefMatch = hrefRegex.match(linkElement);

        if (hrefMatch.hasMatch()) {
            QUrl newUrl = makeUrlFromHref(feed.xmlUrl(), hrefMatch.captured(1));

            planReFetch(feedFetcher, QUrl(newUrl));
            break;
        }
    }
}

static bool findFaviconUrl(const QByteArray &body, Feed &feed, QRegularExpression pattern, QRegularExpression relPattern)
{
    auto it = pattern.globalMatch(QString::fromUtf8(body));

    while (it.hasNext()) {
        auto match = it.next();

        if (match.hasMatch()) {
            QString text = match.captured();

            if (relPattern.match(text).hasMatch()) {
                QString href = match.captured(QStringLiteral("url"));
                QUrl faviconUrl = makeUrlFromHref(feed.xmlUrl(), href);

                qDebug() << "Favicon found at" << faviconUrl;
                feed.setFaviconUrl(faviconUrl);
                return true;
            }
        }
    }
    return false;
}

void probeHtmlForFavicon(const QByteArray &body, Feed &feed)
{
    QRegularExpression patternDoubleQuotes(
        // Source: <\s*link\s+[^>]*href="(?<url>[^']+)"[^>]*>
        QStringLiteral("<\\s*link\\s+[^>]*href=\"(?<url>[^\"]+)\"[^>]*>"),
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpression relPatternDoubleQuotes(QStringLiteral("\\s+rel=\"[^\"]*icon[^\"]*\""), QRegularExpression::CaseInsensitiveOption);

    QRegularExpression patternSingleQuotes(QStringLiteral("<\\s*link\\s+[^>]*href='(?<url>[^']+)'[^>]*>"), QRegularExpression::CaseInsensitiveOption);
    QRegularExpression relPatternSingleQuotes(QStringLiteral("\\s+rel='[^']*icon[^']*'"), QRegularExpression::CaseInsensitiveOption);

    qDebug() << "Probing" << feed.xmlUrl() << "for favicon...";

    if (!findFaviconUrl(body, feed, patternDoubleQuotes, relPatternDoubleQuotes) && !findFaviconUrl(body, feed, patternSingleQuotes, relPatternSingleQuotes)) {
        qDebug() << "Feed::loadFavicon: No favicon match";
    }
}

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

            feedFetcher.redirectTo(QUrl(newUrl));
            break;
        }
    }
}

void probeHtmlForFavicon(const QByteArray &body, Feed &feed)
{
    QRegularExpression pattern(
        // Source: <\s*link\s+(rel="[^"]*icon[^"]*"\s+)?href="(?<url>[^"]+)"(\s*rel="[^"]*icon[^"]*"\s+)?
        QStringLiteral("<\\s*link\\s+(rel=\"[^\"]*icon[^\"]*\"\\s+)?href=\"(?<url>[^\"]+)\"(\\s*rel=\"[^\"]*icon[^\"]*\"\\s+)?"),
        QRegularExpression::CaseInsensitiveOption);
    auto match = pattern.match(QString::fromUtf8(body));

    if (match.hasMatch()) {
        QString href = match.captured(QStringLiteral("url"));
        QUrl faviconUrl = makeUrlFromHref(feed.xmlUrl(), href);

        feed.setFaviconUrl(faviconUrl);
    } else
        qDebug() << "Feed::loadFavicon: No favicon match";
}

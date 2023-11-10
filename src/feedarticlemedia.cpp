#include "feedarticlemedia.h"
#include <QDomElement>

FeedArticleMedia::FeedArticleMedia(QObject *parent)
    : QObject(parent)
{
}

void FeedArticleMedia::loadFromXml(const QDomElement &element)
{
    QDomElement titleElement = element.firstChildElement(QStringLiteral("media:title"));
    QDomElement contentElement = element.firstChildElement(QStringLiteral("media:content"));
    QDomElement thumbnailElement = element.firstChildElement(QStringLiteral("media:thumbnail"));
    QDomElement descriptionElement = element.firstChildElement(QStringLiteral("media:description"));
    QDomElement communityElement = element.firstChildElement(QStringLiteral("media:community"));
    QDomElement starRatingElement = communityElement.firstChildElement(QStringLiteral("media:starRating"));
    QDomElement statisticsElement = communityElement.firstChildElement(QStringLiteral("media:statistics"));

    setTitle(titleElement.text());
    setDescription(descriptionElement.text());

    // Content
    if (contentElement.isNull()) {
        setContentUrl(QUrl());
        setContentSize(QSize(0, 0));
    } else {
        setContentUrl(QUrl(contentElement.attribute(QStringLiteral("url"))));
        setContentSize(QSize(contentElement.attribute(QStringLiteral("width")).toInt(), contentElement.attribute(QStringLiteral("height")).toInt()));
    }

    // Thumbnail
    if (thumbnailElement.isNull()) {
        setThumbnailUrl(QUrl());
        setThumbnailSize(QSize(0, 0));
    } else {
        setThumbnailUrl(QUrl(thumbnailElement.attribute(QStringLiteral("url"))));
        setThumbnailSize(QSize(thumbnailElement.attribute(QStringLiteral("width")).toInt(), thumbnailElement.attribute(QStringLiteral("height")).toInt()));
    }

    // starRating
    if (starRatingElement.isNull())
        setStarRating(0);
    else {
        double average = starRatingElement.attribute(QStringLiteral("average")).toDouble();
        qint64 max = starRatingElement.attribute(QStringLiteral("max")).toInt();

        setStarRating(average / max);
    }

    // viewCount
    if (statisticsElement.isNull() || !statisticsElement.hasAttribute(QStringLiteral("views")))
        setViewCount(0);
    else
        setViewCount(statisticsElement.attribute(QStringLiteral("views")).toInt());
}

void FeedArticleMedia::setTitle(const QString &value)
{
    if (value != m_title) {
        m_title = value;
        Q_EMIT titleChanged(value);
    }
}

void FeedArticleMedia::setContentUrl(const QUrl &value)
{
    if (value != m_contentUrl) {
        m_contentUrl = value;
        Q_EMIT contentUrlChanged(value);
    }
}

void FeedArticleMedia::setContentSize(QSize value)
{
    if (value != m_contentSize) {
        m_contentSize = value;
        Q_EMIT contentSizeChanged(value);
    }
}

void FeedArticleMedia::setThumbnailUrl(const QUrl &value)
{
    if (value != m_thumbnailUrl) {
        m_thumbnailUrl = value;
        Q_EMIT thumbnailUrlChanged(value);
    }
}

void FeedArticleMedia::setThumbnailSize(QSize value)
{
    if (value != m_thumbnailSize) {
        m_thumbnailSize = value;
        Q_EMIT thumbnailSizeChanged(value);
    }
}

void FeedArticleMedia::setDescription(const QString &value)
{
    if (value != m_description) {
        m_description = value;
        Q_EMIT descriptionChanged(value);
    }
}

void FeedArticleMedia::setStarRating(double value)
{
    if (value != m_starRating) {
        m_starRating = value;
        Q_EMIT starRatingChanged(value);
    }
}

void FeedArticleMedia::setViewCount(qint64 value)
{
    if (value != m_viewCount) {
        m_viewCount = value;
        Q_EMIT viewCountChanged(value);
    }
}

QString FeedArticleMedia::youtubeVideoTag() const
{
    return m_contentUrl.path().split(QStringLiteral("/")).last();
}

QUrl FeedArticleMedia::qmlView() const
{
    if (m_contentUrl.host() == QStringLiteral("www.youtube.com"))
        return QUrl(QStringLiteral("qrc:/ArticleYoutube.qml"));
    return QUrl(QStringLiteral("qrc:/ArticleMedia.qml"));
}

#ifndef FEEDARTICLEMEDIA_H
#define FEEDARTICLEMEDIA_H

#include "feedattachment.h"
#include <QSize>
#include <QUrl>
#include <qobject.h>

class QDomElement;

class FeedArticleMedia : public FeedAttachment
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QUrl contentUrl READ contentUrl WRITE setContentUrl NOTIFY contentUrlChanged)
    Q_PROPERTY(QSize contentSize READ contentSize WRITE setContentSize NOTIFY contentSizeChanged)
    Q_PROPERTY(QUrl embedUrl READ embedUrl WRITE setEmbedUrl NOTIFY embedUrlChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl WRITE setThumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString descriptionAsRichText READ descriptionAsRichText NOTIFY descriptionChanged)
    Q_PROPERTY(double starRating READ starRating WRITE setStarRating NOTIFY starRatingChanged)
    Q_PROPERTY(qint64 viewCount READ viewCount WRITE setViewCount NOTIFY viewCountChanged)
public:
    FeedArticleMedia(QObject *parent = nullptr);

    void saveToJson(QJsonObject &) const override;
    void loadFromJson(const QJsonObject &) override;

    void loadFromXml(const QDomElement &);
    Q_INVOKABLE QUrl qmlView() const;
    Q_INVOKABLE QString youtubeVideoTag() const;
    QString descriptionAsRichText() const;

    const QString &title() const
    {
        return m_title;
    }
    const QUrl &contentUrl() const
    {
        return m_contentUrl;
    }
    QSize contentSize() const
    {
        return m_contentSize;
    }
    const QUrl &embedUrl() const
    {
        return m_embedUrl;
    }
    const QUrl &thumbnailUrl() const
    {
        return m_thumbnailUrl;
    }
    QSize thumbnailSize() const
    {
        return m_thumbnailSize;
    }
    const QString &description() const
    {
        return m_description;
    }
    double starRating() const
    {
        return m_starRating;
    }
    qint64 viewCount() const
    {
        return m_viewCount;
    }

public Q_SLOTS:
    void setTitle(const QString &);
    void setContentUrl(const QUrl &);
    void setContentSize(QSize);
    void setEmbedUrl(const QUrl &);
    void setThumbnailUrl(const QUrl &);
    void setThumbnailSize(QSize);
    void setDescription(const QString &);
    void setStarRating(double);
    void setViewCount(qint64);

Q_SIGNALS:
    void titleChanged(const QString &title);
    void contentUrlChanged(const QUrl &contentUrl);
    void contentSizeChanged(QSize contentSize);
    void embedUrlChanged(const QUrl &embedUrl);
    void thumbnailUrlChanged(const QUrl &thumbnailUrl);
    void thumbnailSizeChanged(QSize thumbnailSize);
    void descriptionChanged(const QString &description);
    void starRatingChanged(double starRating);
    void viewCountChanged(qint64 viewCount);

private:
    QString m_title;
    QUrl m_contentUrl;
    QSize m_contentSize;
    QUrl m_embedUrl;
    QUrl m_thumbnailUrl;
    QSize m_thumbnailSize;
    QString m_description;
    double m_starRating;
    qint64 m_viewCount;
};

#endif

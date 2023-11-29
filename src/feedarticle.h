#ifndef FEEDARTICLE_H
#define FEEDARTICLE_H

#include <QDateTime>
#include <QQmlListProperty>
#include <QUrl>
#include <qobject.h>

class QDomElement;
class QJsonObject;

class FeedArticle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool read READ isRead WRITE setRead NOTIFY readChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QUrl authorUrl READ authorUrl WRITE setAuthorUrl NOTIFY authorUrlChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QUrl comments READ comments WRITE setComments NOTIFY commentsChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString guid READ guid WRITE setGuid NOTIFY guidChanged)
    Q_PROPERTY(QUrl link READ link WRITE setLink NOTIFY linkChanged)
    Q_PROPERTY(QDateTime publicationDate READ publicationDate WRITE setPublicationDate NOTIFY publicationDateChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QQmlListProperty<QObject> medias READ qmlMedias NOTIFY mediasChanged)
public:
    friend class RssFeedReader;
    friend class AtomFeedReader;

    FeedArticle(QObject *parent = nullptr);
    ~FeedArticle();

    void loadFromJson(QJsonObject &);
    void saveToJson(QJsonObject &);

    bool isRead() const
    {
        return m_read;
    }
    QString author() const;
    QUrl authorUrl() const;
    QString category() const;
    QUrl comments() const;
    QString description() const;
    QString guid() const;
    QUrl link() const;
    QDateTime publicationDate() const;
    QUrl source() const;
    QString title() const;

    void loadFromAtom(const QDomElement &);

public Q_SLOTS:
    void setRead(bool read);
    void setAuthor(const QString &author);
    void setAuthorUrl(const QUrl &authorUrl);
    void setCategory(const QString &category);
    void setComments(const QUrl &comments);
    void setDescription(const QString &description);
    void setGuid(const QString &guid);
    void setLink(const QUrl &link);
    void setPublicationDate(const QDateTime &publicationDate);
    void setSource(const QUrl &source);
    void setTitle(const QString &title);

Q_SIGNALS:
    void readChanged(bool read);
    void authorChanged(const QString &author);
    void authorUrlChanged(const QUrl &authorUrl);
    void categoryChanged(const QString &category);
    void commentsChanged(const QUrl &comments);
    void descriptionChanged(const QString &description);
    void guidChanged(const QString &guid);
    void linkChanged(const QUrl &link);
    void publicationDateChanged(const QDateTime &publicationDate);
    void sourceChanged(const QUrl &source);
    void titleChanged(const QString &title);
    void mediasChanged();

private:
    QQmlListProperty<QObject> qmlMedias()
    {
        return QQmlListProperty<QObject>(this, reinterpret_cast<QList<QObject *> *>(&m_medias));
    }
    void clearMedias();

    bool m_read = false;
    QString m_author;
    QUrl m_authorUrl;
    QString m_category;
    QUrl m_comments;
    QString m_description;
    QString m_guid;
    QUrl m_link;
    QDateTime m_publicationDate;
    QUrl m_source;
    QString m_title;
    QList<QObject *> m_medias;
};

#endif // FEEDARTICLE_H

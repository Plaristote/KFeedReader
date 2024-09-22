#ifndef FEED_H
#define FEED_H

#include "feedupdate.h"
#include "menuitem.h"
#include <QDateTime>
#include <QQmlListProperty>
#include <QTimer>
#include <QUrl>

class FeedArticle;
class QNetworkAccessManager;
class QNetworkReply;
class QDomNode;
class QDomElement;
class QJsonArray;

class Feed : public MenuItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl faviconUrl READ faviconUrl MEMBER m_faviconUrl NOTIFY faviconUrlChanged)
    Q_PROPERTY(QUrl xmlUrl MEMBER m_xmlUrl NOTIFY xmlUrlChanged)
    Q_PROPERTY(int maxArticleAge MEMBER m_maxArticleAge NOTIFY maxArticleAgeChanged)
    Q_PROPERTY(int maxArticleNumber MEMBER m_maxArticleNumber NOTIFY maxArticleNumberChanged)
    Q_PROPERTY(QQmlListProperty<QObject> articles READ qmlArticles NOTIFY articlesChanged)

    Q_PROPERTY(QDateTime publicationDate READ publicationDate WRITE setPublicationDate NOTIFY publicationDateChanged)
    Q_PROPERTY(QString managingEditor READ managingEditor WRITE setManagingEditor NOTIFY managingEditorChanged)
    Q_PROPERTY(QUrl link READ link WRITE setLink NOTIFY linkChanged)
    Q_PROPERTY(QUrl logoUrl READ logoUrl WRITE setLogoUrl NOTIFY logoUrlChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString generator READ generator WRITE setGenerator NOTIFY generatorChanged)
    Q_PROPERTY(QString copyright READ copyright WRITE setCopyright NOTIFY copyrightChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(bool hasTextInput READ hasTextInput NOTIFY textInputChanged)
    Q_PROPERTY(QString textInputDescription READ textInputDescription WRITE setTextInputDescription NOTIFY textInputDescriptionChanged)
    Q_PROPERTY(QString textInputTitle READ textInputTitle WRITE setTextInputTitle NOTIFY textInputTitleChanged)
    Q_PROPERTY(QString textInputName READ textInputName WRITE setTextInputName NOTIFY textInputNameChanged)
    Q_PROPERTY(QUrl textInputLink READ textInputLink WRITE setTextInputLink NOTIFY textInputLinkChanged)
    Q_PROPERTY(QDateTime lastBuildDate READ lastBuildDate WRITE setLastBuildDate NOTIFY lastBuildDateChanged)
    Q_PROPERTY(QString webmaster READ webmaster WRITE setWebmaster NOTIFY webmasterChanged)
    Q_PROPERTY(int ttl READ ttl WRITE setTtl NOTIFY ttlChanged)
    Q_PROPERTY(QDateTime lastUpdate READ lastUpdate WRITE setLastUpdate NOTIFY lastUpdateChanged)
    Q_PROPERTY(QDateTime scheduledUpdate READ scheduledUpdate WRITE setScheduledUpdate NOTIFY scheduledUpdateChanged)
public:
    friend class AtomFeedReader;
    friend class RssFeedReader;
    friend class RdfFeedReader;
    friend class JsonFeedReader;
    friend class FeedUpdater;
    friend class FeedFetcher;
    friend class FeedFavicon;
    friend class FeedYoutubeFavicon;

    enum FeedType { HTMLView, RDFFeed, RSSFeed, AtomFeed, JSONFeed };
    Q_ENUM(FeedType)

    Feed(QObject *parent = nullptr);
    ~Feed();

    QString view() const override
    {
        return QStringLiteral("qrc:/Feed.qml");
    }
    QString storagePath() const;
    QString storagePrefix() const;
    QString faviconStoragePath() const;
    QUrl faviconUrl() const override;
    QUrl remoteFaviconUrl() const;
    FeedArticle *findArticleByGuid(const QString &guid) const;
    FeedArticle *findArticleByLink(const QUrl &link) const;
    Q_INVOKABLE FeedArticle *findNextArticle(const FeedArticle *);
    Q_INVOKABLE FeedArticle *findPreviousArticle(const FeedArticle *);
    qint64 unreadCount() const override;
    void markAsRead() override;
    bool fetching() const override
    {
        return m_fetching;
    }
    double progress() const override
    {
        return m_progress;
    }
    ItemType itemType() const override
    {
        return MenuItem::FeedMenuItem;
    }

    QStringList persistentProperties() const;
    void loadFromJson(QJsonObject &) override;
    void saveToJson(QJsonObject &) const override;
    void saveArticlesToJson(QJsonArray &) const;
    void loadArticlesFromJson(const QJsonArray &);
    static Feed *createFromJson(QJsonObject &);

    bool hasTextInput() const
    {
        return m_textInputName.length() > 0;
    }
    const QUrl &xmlUrl() const
    {
        return m_xmlUrl;
    }
    int maxArticleAge() const
    {
        return m_maxArticleAge;
    }
    int maxArticleNumber() const
    {
        return m_maxArticleNumber;
    }
    const QString &uuid() const
    {
        return m_uuid;
    }
    const QDateTime &publicationDate() const
    {
        return m_publicationDate;
    }
    const QString &managingEditor() const
    {
        return m_managingEditor;
    }
    const QUrl &link() const
    {
        return m_link;
    }
    const QUrl &logoUrl() const
    {
        return m_logoUrl;
    }
    const QString &language() const
    {
        return m_language;
    }
    const QString &generator() const
    {
        return m_generator;
    }
    const QString &copyright() const
    {
        return m_copyright;
    }
    const QString &category() const
    {
        return m_category;
    }
    const QString &textInputDescription() const
    {
        return m_textInputDescription;
    }
    const QString &textInputTitle() const
    {
        return m_textInputTitle;
    }
    const QString &textInputName() const
    {
        return m_textInputName;
    }
    const QUrl &textInputLink() const
    {
        return m_textInputLink;
    }
    const QDateTime &lastBuildDate() const
    {
        return m_lastBuildDate;
    }
    const QString &title() const
    {
        return name();
    }
    const QString &webmaster() const
    {
        return m_webmaster;
    }
    int ttl() const
    {
        return m_ttl;
    }
    const QDateTime &scheduledUpdate() const
    {
        return m_scheduledUpdate;
    }
    const QDateTime &lastUpdate() const
    {
        return m_lastUpdate;
    }
    const QList<FeedArticle *> &articles() const
    {
        return m_articles;
    }

    void setUuid(const QString &value)
    {
        m_uuid = value;
    }

    void setFaviconUrl(const QUrl &value);
    void setRemoteFaviconUrl(const QUrl &value);

    bool isSkippedHour(unsigned short index) const override;
    bool isSkippedDay(unsigned short index) const override;

public Q_SLOTS:
    void remove() override;
    void loadArticleFile();
    void saveArticleFile() const;
    void fetch() override;
    void setXmlUrl(const QUrl &);
    void setPublicationDate(const QDateTime &);
    void setManagingEditor(const QString &);
    void setLink(const QUrl &);
    void setLogoUrl(const QUrl &);
    void setLanguage(const QString &);
    void setGenerator(const QString &);
    void setCopyright(const QString &);
    void setCategory(const QString &);
    void setTextInputDescription(const QString &);
    void setTextInputTitle(const QString &);
    void setTextInputName(const QString &);
    void setTextInputLink(const QUrl &);
    void setLastBuildDate(const QDateTime &);
    void setWebmaster(const QString &);
    void setTtl(int);
    void setScheduledUpdate(const QDateTime &);
    void setLastUpdate(const QDateTime &);
    void loadFaviconFrom(const QUrl &);
    void copy(const Feed *);

Q_SIGNALS:
    void faviconUrlChanged();
    void xmlUrlChanged();
    void htmlUrlChanged();
    void maxArticleAgeChanged();
    void maxArticleNumberChanged();

    void publicationDateChanged();
    void managingEditorChanged();
    void linkChanged();
    void logoUrlChanged();
    void languageChanged();
    void generatorChanged();
    void copyrightChanged();
    void categoryChanged();
    void textInputDescriptionChanged();
    void textInputTitleChanged();
    void textInputNameChanged();
    void textInputLinkChanged();
    void textInputChanged();
    void lastBuildDateChanged();
    void webmasterChanged();
    void ttlChanged();
    void scheduledUpdateChanged();
    void lastUpdateChanged();

    void articlesChanged();
    void requestFaviconUpdate(QUrl);

private:
    QQmlListProperty<QObject> qmlArticles()
    {
        return QQmlListProperty<QObject>(this, reinterpret_cast<QList<QObject *> *>(&m_articles));
    }

    FeedArticle *newArticle();
    void insertArticle(FeedArticle *);

    QString m_uuid;
    QUrl m_faviconUrl;
    QUrl m_remoteFaviconUrl;
    QUrl m_xmlUrl;
    int m_maxArticleAge = 0;
    int m_maxArticleNumber = 0;
    QList<FeedArticle *> m_articles;
    QVector<unsigned short> m_skipDays;
    QVector<unsigned short> m_skipHours;
    QDateTime m_lastBuildDate;
    QDateTime m_publicationDate;
    QString m_managingEditor;
    QUrl m_link;
    QUrl m_logoUrl;
    QString m_language;
    QString m_generator;
    QString m_copyright;
    QString m_category;
    QString m_textInputDescription;
    QString m_textInputTitle;
    QUrl m_textInputLink;
    QString m_textInputName;
    QString m_webmaster;
    QNetworkAccessManager *m_network;
    bool m_fetching = false;
    double m_progress = 0;
    QDateTime m_lastUpdate;
    QDateTime m_scheduledUpdate;
    FeedUpdater m_feedUpdater;
    int m_ttl = 0;
};

#endif // FEED_H

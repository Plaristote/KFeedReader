#include "feed.h"
#include "feedarticle.h"
#include "feedfetch.h"
#include "reader-atom.h"
#include "reader-json.h"
#include "reader-rss.h"
#include <QDomDocument>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>

void probeHtmlForFavicon(const QByteArray &body, Feed &feed);

Feed::Feed(QObject *parent)
    : MenuItem(parent)
    , m_feedUpdater(*this)
{
    m_uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_network = new QNetworkAccessManager();
    m_network->setRedirectPolicy(QNetworkRequest::SameOriginRedirectPolicy);
    // connect(this, &Feed::xmlUrlChanged, this, &Feed::fetch);
    connect(this, &Feed::textInputDescriptionChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::textInputTitleChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::textInputNameChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::textInputLinkChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::articlesChanged, this, &Feed::unreadCountChanged);
    connect(this, &Feed::requestFaviconUpdate, this, &Feed::loadImageFromUrl, Qt::QueuedConnection);
}

Feed::~Feed()
{
    m_network->deleteLater();
}

QString Feed::storagePrefix() const
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return folder + QStringLiteral("/") + m_uuid;
}

QString Feed::storagePath() const
{
    return storagePrefix() + QStringLiteral(".json");
}

void Feed::remove()
{
    QFile::remove(storagePath());
    MenuItem::remove();
}

QStringList Feed::persistentProperties() const
{
    return {QStringLiteral("xmlUrl"),
            QStringLiteral("lastUpdate"),
            QStringLiteral("ttl"),
            QStringLiteral("customTtl"),
            QStringLiteral("useCustomTtl"),
            QStringLiteral("faviconUrl"),
            QStringLiteral("link"),
            QStringLiteral("lastBuildDate"),
            QStringLiteral("publicationDate"),
            QStringLiteral("category"),
            QStringLiteral("webmaster")};
}

static const QString uuidKey = QStringLiteral("uuid");

void Feed::loadFromJson(QJsonObject &root)
{
    m_uuid = root[uuidKey].toString();
    MenuItem::loadFromJson(root);
    for (const auto &key : persistentProperties()) {
        QJsonValue value = root[key];

        if (!value.isNull())
            setProperty(key.toUtf8().constData(), root[key].toVariant());
    }
    loadArticleFile();
    m_feedUpdater.restartUpdateTimer();
}

void Feed::loadArticleFile()
{
    QFile file(storagePath());

    if (file.open(QIODevice::ReadOnly)) {
        QJsonArray articlesJson = QJsonDocument::fromJson(file.readAll()).array();

        qDebug() << "Successfully loaded article JSON";
        for (auto *article : m_articles)
            article->deleteLater();
        m_articles.clear();
        for (QJsonValue articleJson : articlesJson) {
            FeedArticle *article = newArticle();
            QJsonObject articleJsonObject = articleJson.toObject();

            article->loadFromJson(articleJsonObject);
            if (!findArticleByGuid(article->guid()))
                m_articles << article;
            else
                delete article;
        }
        Q_EMIT articlesChanged();
    } else
        qDebug() << "Failed to load JSON article file" << storagePath();
}

void Feed::copy(const Feed *source)
{
    if (source) {
        QJsonObject json;

        source->saveToJson(json);
        loadFromJson(json);
    }
}

void Feed::saveToJson(QJsonObject &root) const
{
    root[uuidKey] = m_uuid;
    MenuItem::saveToJson(root);
    for (const auto &key : persistentProperties())
        root.insert(key, QJsonValue::fromVariant(property(key.toUtf8().constData())));
    saveArticleFile();
}

void Feed::saveArticleFile() const
{
    QFile file(storagePath());

    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray articlesJson;

        for (FeedArticle *article : m_articles) {
            QJsonObject articleJson;

            article->saveToJson(articleJson);
            articlesJson << articleJson;
        }
        file.write(QJsonDocument(articlesJson).toJson());
    }
}

Feed *Feed::createFromJson(QJsonObject &root, QObject *parent)
{
    Feed *item = new Feed(parent);

    item->loadFromJson(root);
    return item;
}

void Feed::fetch()
{
    FeedFetcher *fetcher = new FeedFetcher(*this);

    fetcher->fetch();
}

void Feed::setFaviconUrl(const QUrl &value)
{
    QNetworkRequest request(value);
    QNetworkReply *reply = m_network->get(request);

    qDebug() << "Fetching favicon" << value;
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        qDebug() << "Received favicon" << status;
        if (status >= 200 && status < 300) {
            QImage image;

            if (image.loadFromData(reply->readAll())) {
                image.save(faviconStoragePath(), "png");
                Q_EMIT faviconUrlChanged();
            }
        }
    });
}

QString Feed::faviconStoragePath() const
{
    return storagePrefix() + QStringLiteral(".png");
}

QUrl Feed::faviconUrl() const
{
    QString path = faviconStoragePath();

    if (QFile::exists(path))
        return QUrl(QStringLiteral("file:") + path);
    return QUrl(QStringLiteral("qrc:/icons/feed.png"));
}

void Feed::setXmlUrl(const QUrl &value)
{
    if (value != m_xmlUrl) {
        m_xmlUrl = value;
        Q_EMIT xmlUrlChanged();
    }
}

void Feed::setPublicationDate(const QDateTime &value)
{
    if (value != m_publicationDate) {
        m_publicationDate = value;
        Q_EMIT publicationDateChanged();
    }
}

void Feed::setManagingEditor(const QString &value)
{
    if (value != m_managingEditor) {
        m_managingEditor = value;
        Q_EMIT managingEditorChanged();
    }
}

void Feed::setLink(const QUrl &value)
{
    if (value != m_link) {
        m_link = value;
        Q_EMIT linkChanged();
    }
}

void Feed::setLanguage(const QString &value)
{
    if (value != m_language) {
        m_language = value;
        Q_EMIT languageChanged();
    }
}

void Feed::setGenerator(const QString &value)
{
    if (value != m_generator) {
        m_generator = value;
        Q_EMIT generatorChanged();
    }
}

void Feed::setCopyright(const QString &value)
{
    if (value != m_copyright) {
        m_copyright = value;
        Q_EMIT copyrightChanged();
    }
}

void Feed::setCategory(const QString &value)
{
    if (value != m_category) {
        m_category = value;
        Q_EMIT categoryChanged();
    }
}

void Feed::setTextInputDescription(const QString &value)
{
    if (value != m_textInputDescription) {
        m_textInputDescription = value;
        Q_EMIT textInputDescriptionChanged();
    }
}

void Feed::setTextInputTitle(const QString &value)
{
    if (value != m_textInputTitle) {
        m_textInputTitle = value;
        Q_EMIT textInputTitleChanged();
    }
}

void Feed::setTextInputName(const QString &value)
{
    if (value != m_textInputName) {
        m_textInputName = value;
        Q_EMIT textInputNameChanged();
    }
}

void Feed::setTextInputLink(const QUrl &value)
{
    if (value != m_textInputLink) {
        m_textInputLink = value;
        Q_EMIT textInputLinkChanged();
    }
}

void Feed::setLastBuildDate(const QDateTime &value)
{
    if (value != m_lastBuildDate) {
        m_lastBuildDate = value;
        Q_EMIT lastBuildDateChanged();
    }
}

void Feed::setWebmaster(const QString &value)
{
    if (value != m_webmaster) {
        m_webmaster = value;
        Q_EMIT webmasterChanged();
    }
}

void Feed::setTtl(int value)
{
    if (value != m_ttl) {
        m_ttl = value;
        Q_EMIT ttlChanged();
    }
}

void Feed::setCustomTtl(int value)
{
    if (value != m_customTtl) {
        m_customTtl = value;
        Q_EMIT customTtlChanged();
    }
}

void Feed::setUseCustomTtl(bool value)
{
    if (value != m_useCustomTtl) {
        m_useCustomTtl = value;
        Q_EMIT customTtlChanged();
    }
}

void Feed::setLastUpdate(const QDateTime &value)
{
    if (value != m_lastUpdate) {
        m_lastUpdate = value;
        Q_EMIT lastUpdateChanged();
    }
}

void Feed::setScheduledUpdate(const QDateTime &value)
{
    if (value != m_scheduledUpdate) {
        m_scheduledUpdate = value;
        Q_EMIT scheduledUpdateChanged();
    }
}

void Feed::loadFaviconFrom(const QUrl &remoteUrl, unsigned char redirectCount)
{
    static const unsigned char maxRedirectCount = 10;
    QNetworkReply *reply = m_network->get(QNetworkRequest(remoteUrl));

    qDebug() << "Feed::loadFavicon: Fetching favicon from HTML" << remoteUrl;
    reply->ignoreSslErrors();
    connect(reply, &QNetworkReply::finished, this, [this, reply, redirectCount, remoteUrl]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        qDebug() << "Feed::loadFavicon: Received response" << status;
        if (status == 302 && redirectCount < maxRedirectCount)
            loadFaviconFrom(QUrl(reply->header(QNetworkRequest::LocationHeader).toString()), redirectCount + 1);
        else if (status >= 200 && status < 300) {
            probeHtmlForFavicon(reply->readAll(), *this);
        }
        reply->deleteLater();
    });
}

void Feed::loadImageFromUrl(const QUrl &remoteUrl)
{
    qDebug() << "loadImageFromUrl";
    if (remoteUrl.toString().length() == 0)
        loadFaviconFrom(m_link);
    else
        m_faviconUrl = remoteUrl;
    qDebug() << "/!\\ TODO: Ignoring feed icon" << remoteUrl;
}

void Feed::insertArticle(FeedArticle *article)
{
    QDateTime publicationDate = article->publicationDate();

    for (auto it = m_articles.begin(); it != m_articles.end(); ++it) {
        if ((*it)->publicationDate() < publicationDate) {
            m_articles.insert(it, article);
            return;
        }
    }
    m_articles << article;
}

FeedArticle *Feed::newArticle()
{
    auto *article = new FeedArticle(this);

    connect(article, &FeedArticle::readChanged, this, &Feed::unreadCountChanged);
    return article;
}

FeedArticle *Feed::findArticleByGuid(const QString &guid) const
{
    if (!guid.isEmpty()) {
        for (FeedArticle *feed : m_articles) {
            if (feed->guid() == guid)
                return feed;
        }
    }
    return nullptr;
}

FeedArticle *Feed::findArticleByLink(const QUrl &link) const
{
    for (FeedArticle *feed : m_articles) {
        if (feed->link() == link)
            return feed;
    }
    return nullptr;
}

qint64 Feed::unreadCount() const
{
    qint64 total = 0;

    for (const FeedArticle *article : m_articles)
        total += !article->isRead() ? 1 : 0;
    return total;
}

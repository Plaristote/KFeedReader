#include "feed.h"
#include "feedarticle.h"
#include <QDomDocument>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUuid>

Feed::Feed(QObject *parent)
    : MenuItem(parent)
{
    m_uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_network = new QNetworkAccessManager();
    m_network->setRedirectPolicy(QNetworkRequest::SameOriginRedirectPolicy);
    connect(this, &Feed::xmlUrlChanged, this, &Feed::fetch);
    connect(this, &Feed::textInputDescriptionChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::textInputTitleChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::textInputNameChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::textInputLinkChanged, this, &Feed::textInputChanged);
    connect(this, &Feed::articlesChanged, this, &Feed::unreadCountChanged);
    connect(this, &Feed::requestFaviconUpdate, this, &Feed::loadImageFromUrl, Qt::QueuedConnection);
    connect(&m_updateTimer, &QTimer::timeout, this, &Feed::fetch);
}

Feed::~Feed()
{
    m_network->deleteLater();
}

QString Feed::storagePath() const
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return folder + QStringLiteral("/") + m_uuid + QStringLiteral(".json");
}

void Feed::remove()
{
    QFile::remove(storagePath());
    MenuItem::remove();
}

void Feed::resetUpdateTimer()
{
    qint64 ttl = m_ttl > 0 ? m_ttl : 60; // TODO set default ttl using QSettings

    m_scheduledUpdate = QDateTime::currentDateTime().addSecs(ttl * 60);
    restartUpdateTimer();
}

void Feed::restartUpdateTimer()
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 interval;

    if (now < m_scheduledUpdate)
        interval = now.msecsTo(m_scheduledUpdate);
    else
        interval = 500;
    m_updateTimer.stop();
    m_updateTimer.setInterval(interval);
    m_updateTimer.start();
}

void Feed::loadFromJson(QJsonObject &root)
{
    MenuItem::loadFromJson(root);
    m_uuid = root.value(QStringLiteral("uuid")).toString();
    m_xmlUrl = (QUrl(root.value(QStringLiteral("xmlUrl")).toString()));
    m_scheduledUpdate = QDateTime::fromString(root.value(QStringLiteral("scheduledUpdate")).toString(), Qt::ISODate);
    loadArticleFile();
    restartUpdateTimer();
}

void Feed::loadArticleFile()
{
    QFile file(storagePath());

    if (file.open(QIODevice::ReadOnly)) {
        QJsonArray articlesJson = QJsonDocument::fromJson(file.readAll()).array();

        for (QJsonValue articleJson : articlesJson) {
            FeedArticle *article = newArticle();
            QJsonObject articleJsonObject = articleJson.toObject();

            article->loadFromJson(articleJsonObject);
            m_articles << article;
        }
        Q_EMIT articlesChanged();
    }
}

void Feed::saveToJson(QJsonObject &root)
{
    MenuItem::saveToJson(root);
    root.insert(QStringLiteral("uuid"), m_uuid);
    root.insert(QStringLiteral("xmlUrl"), m_xmlUrl.toString());
    root.insert(QStringLiteral("scheduledUpdate"), m_scheduledUpdate.toString(Qt::ISODate));
    saveArticleFile();
}

void Feed::saveArticleFile()
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

static Feed::FeedType inferFeedType(QNetworkReply &reply, const QDomDocument &document)
{
    QString contentType = reply.header(QNetworkRequest::ContentTypeHeader).toString();

    qDebug() << "> Loading feed with content type" << contentType;
    if (contentType.contains(QStringLiteral("atom+xml")))
        return Feed::AtomFeed;
    else if (contentType.contains(QStringLiteral("rss+xml")))
        return Feed::RSSFeed;
    else if (!document.firstChildElement(QStringLiteral("feed")).isNull())
        return Feed::AtomFeed;
    return Feed::RSSFeed;
}

void Feed::fetch()
{
    QNetworkReply *reply = m_network->get(QNetworkRequest(m_xmlUrl));

    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesRead, qint64 totalBytes) {
        m_progress = static_cast<double>(bytesRead) / totalBytes;
        Q_EMIT progressChanged();
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        unsigned int status = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toUInt();

        m_progress = 1;
        m_fetching = false;
        if (status >= 200 && status < 300) {
            QDomDocument document;
            auto body = reply->readAll();

            document.setContent(body);
            switch (inferFeedType(*reply, document)) {
            case RSSFeed:
                loadRSSDocument(document);
                break;
            case AtomFeed:
                loadAtomDocument(document);
                break;
            }
        }
        resetUpdateTimer();
        Q_EMIT fetchingChanged();
    });
    m_fetching = true;
    Q_EMIT fetchingChanged();
}

void Feed::loadAtomDocument(const QDomNode &document)
{
    QDomElement feed = document.firstChildElement(QStringLiteral("feed"));
    QDomElement titleElement = feed.firstChildElement(QStringLiteral("title"));
    QDomElement linkElement = feed.firstChildElement(QStringLiteral("link"));
    QDomElement updatedElement = feed.firstChildElement(QStringLiteral("updated"));
    QDomElement publishedElement = feed.firstChildElement(QStringLiteral("published"));
    QDomElement imageElement = feed.firstChildElement(QStringLiteral("image"));
    QDomElement logoElement = feed.firstChildElement(QStringLiteral("logo"));
    QString linkAttribute;

    qDebug() << "> Loading ATOM feed";
    setName(titleElement.isNull() ? QString() : titleElement.text());
    while (!linkElement.isNull() && linkElement.attribute(QStringLiteral("rel")) != QStringLiteral("alternate"))
        linkElement = linkElement.nextSiblingElement(QStringLiteral("link"));
    linkAttribute = linkElement.attribute(QStringLiteral("href"));
    qDebug() << "Link element = " << linkElement.attribute(QStringLiteral("href"));
    setLink(linkAttribute.isNull() ? QUrl() : QUrl(linkAttribute));
    setLastBuildDate(updatedElement.isNull() ? QDateTime::currentDateTime() : QDateTime::fromString(updatedElement.text()));
    setPublicationDate(publishedElement.isNull() ? QDateTime() : QDateTime::fromString(publishedElement.text(), Qt::ISODate));
    loadAtomArticles(feed);
    if (imageElement.isNull() && !logoElement.isNull())
        Q_EMIT requestFaviconUpdate(QUrl(logoElement.text()));
    else if (!imageElement.isNull())
        Q_EMIT requestFaviconUpdate(QUrl(imageElement.text()));
    else
        Q_EMIT requestFaviconUpdate(QUrl());
}

void Feed::loadRSSDocument(const QDomNode &document)
{
    QDomElement rss = document.firstChildElement(QStringLiteral("rss"));
    QDomElement channel = rss.firstChildElement(QStringLiteral("channel"));
    QDomElement categoryElement = channel.firstChildElement(QStringLiteral("category"));
    QDomElement copyrightElement = channel.firstChildElement(QStringLiteral("copyright"));
    QDomElement descriptionElement = channel.firstChildElement(QStringLiteral("description"));
    QDomElement imageElement = channel.firstChildElement(QStringLiteral("image"));
    QDomElement languageElement = channel.firstChildElement(QStringLiteral("language"));
    QDomElement lastBuildDateElement = channel.firstChildElement(QStringLiteral("lastBuildDate"));
    QDomElement linkElement = channel.firstChildElement(QStringLiteral("link"));
    QDomElement managingEditorElement = channel.firstChildElement(QStringLiteral("managingEditor"));
    QDomElement pubDateElement = channel.firstChildElement(QStringLiteral("pubDate"));
    QDomElement skipDaysElement = channel.firstChildElement(QStringLiteral("skipDays"));
    QDomElement skipHoursElement = channel.firstChildElement(QStringLiteral("skipHours"));
    QDomElement textInputElement = channel.firstChildElement(QStringLiteral("textinput"));
    QDomElement titleElement = channel.firstChildElement(QStringLiteral("title"));
    QDomElement webMasterElement = channel.firstChildElement(QStringLiteral("webMaster"));
    QDomElement ttlElement = channel.firstChildElement(QStringLiteral("ttl"));

    qDebug() << "> Loading RSS feed";
    setCategory(categoryElement.isNull() ? QString() : categoryElement.text());
    setCopyright(copyrightElement.isNull() ? QString() : copyrightElement.text());
    setDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
    setLanguage(languageElement.isNull() ? QString() : languageElement.text());
    setLastBuildDate(lastBuildDateElement.isNull() ? QDateTime::currentDateTime() : QDateTime::fromString(lastBuildDateElement.text()));
    setLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.text()));
    setManagingEditor(managingEditorElement.isNull() ? QString() : managingEditorElement.text());
    setPublicationDate(pubDateElement.isNull() ? QDateTime() : QDateTime::fromString(pubDateElement.text(), Qt::RFC2822Date));
    loadSkipDays(skipDaysElement);
    loadSkipHours(skipHoursElement);
    loadTextInput(textInputElement);
    setName(titleElement.isNull() ? QString() : titleElement.text());
    setWebmaster(webMasterElement.isNull() ? QString() : webMasterElement.text());
    m_ttl = ttlElement.isNull() ? 0 : ttlElement.text().toInt();
    loadRSSArticles(channel);
    Q_EMIT requestFaviconUpdate(imageElement.isNull() ? QUrl() : QUrl(imageElement.text()));
}

QUrl Feed::faviconUrl() const
{
    qDebug() << "Favicon URL:" << m_faviconUrl.toString() << m_faviconUrl.isEmpty(); // << m_faviconUrl.toString().length() == 0
    if (m_faviconUrl.toString().length() == 0)
        return QUrl(QStringLiteral("qrc:/icons/feed.png"));
    return m_faviconUrl;
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
            auto body = reply->readAll();
            QRegularExpression pattern(
                // Source: <\s*link\s+(rel="[^"]*icon[^"]*"\s+)?href="(?<url>[^"]+)"(\s*rel="[^"]*icon[^"]*"\s+)?
                QStringLiteral("<\\s*link\\s+(rel=\"[^\"]*icon[^\"]*\"\\s+)?href=\"(?<url>[^\"]+)\"(\\s*rel=\"[^\"]*icon[^\"]*\"\\s+)?"),
                QRegularExpression::CaseInsensitiveOption);
            auto match = pattern.match(QString::fromUtf8(body));

            if (match.hasMatch()) {
                QUrl faviconUrl(match.captured(QStringLiteral("url")));

                if (!faviconUrl.scheme().length())
                    faviconUrl.setScheme(remoteUrl.scheme());
                if (!faviconUrl.host().length())
                    faviconUrl.setHost(remoteUrl.host());
                m_faviconUrl = faviconUrl;
                Q_EMIT faviconUrlChanged();
            } else
                qDebug() << "Feed::loadFavicon: No favicon match";
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

void Feed::loadSkipDays(const QDomElement &element)
{
    m_skipDays.clear();
    for (QDomElement child = element.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
        static const QHash<QString, unsigned short> dayMap{{QStringLiteral("Monday"), 1},
                                                           {QStringLiteral("Tuesday"), 2},
                                                           {QStringLiteral("Wednesday"), 3},
                                                           {QStringLiteral("Thursday"), 4},
                                                           {QStringLiteral("Friday"), 5},
                                                           {QStringLiteral("Saturday"), 6},
                                                           {QStringLiteral("Sunday"), 7}};
        auto it = dayMap.constFind(child.text());
        unsigned short day = it != dayMap.end() ? it.value() : 0;

        if (day != 0 && m_skipDays.indexOf(day) == -1)
            m_skipDays << day;
    }
    Q_EMIT skipDaysChanged();
}

void Feed::loadSkipHours(const QDomElement &element)
{
    m_skipHours.clear();
    for (QDomElement child = element.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {
        unsigned short hour = child.text().toUShort();

        if (m_skipHours.indexOf(hour) == -1)
            m_skipHours << hour;
    }
    Q_EMIT skipHoursChanged();
}

void Feed::loadTextInput(const QDomElement &element)
{
    QDomElement descriptionElement = element.firstChildElement(QStringLiteral("description"));
    QDomElement nameElement = element.firstChildElement(QStringLiteral("name"));
    QDomElement linkElement = element.firstChildElement(QStringLiteral("link"));
    QDomElement titleElement = element.firstChildElement(QStringLiteral("title"));

    setTextInputDescription(descriptionElement.isNull() ? QString() : descriptionElement.text());
    setTextInputName(nameElement.isNull() ? QString() : nameElement.text());
    setTextInputLink(linkElement.isNull() ? QUrl() : QUrl(linkElement.text()));
    setTextInputTitle(titleElement.isNull() ? QString() : titleElement.text());
}

void Feed::loadAtomArticles(const QDomNode &root)
{
    for (QDomElement itemElement = root.firstChildElement(QStringLiteral("entry")); !itemElement.isNull();
         itemElement = itemElement.nextSiblingElement(QStringLiteral("entry"))) {
        QDomElement idElement = itemElement.firstChildElement(QStringLiteral("id"));
        QDomElement linkElement = itemElement.firstChildElement(QStringLiteral("link"));
        QString guid = idElement.isNull() ? QString() : idElement.text();
        QUrl link = linkElement.isNull() ? QUrl() : QUrl(linkElement.attribute(QStringLiteral("href")));
        FeedArticle *article = !guid.isEmpty() ? findArticleByGuid(guid) : findArticleByLink(link);

        if (article)
            article->loadFromAtom(itemElement);
        else {
            article = newArticle();
            article->loadFromAtom(itemElement);
            insertArticle(article);
        }
    }
    Q_EMIT articlesChanged();
}

void Feed::loadRSSArticles(const QDomNode &root)
{
    for (QDomElement itemElement = root.firstChildElement(QStringLiteral("item")); !itemElement.isNull();
         itemElement = itemElement.nextSiblingElement(QStringLiteral("item"))) {
        QDomElement guidElement = itemElement.firstChildElement(QStringLiteral("guid"));
        QDomElement linkElement = itemElement.firstChildElement(QStringLiteral("link"));
        QString guid = guidElement.isNull() ? QString() : guidElement.text();
        QUrl link = linkElement.isNull() ? QUrl() : QUrl(linkElement.text());
        FeedArticle *article = !guid.isEmpty() ? findArticleByGuid(guid) : findArticleByLink(link);

        if (article)
            article->loadFromRSS(itemElement);
        else {
            article = newArticle();
            article->loadFromRSS(itemElement);
            insertArticle(article);
        }
    }
    Q_EMIT articlesChanged();
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
    for (FeedArticle *feed : m_articles) {
        if (feed->guid() == guid)
            return feed;
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

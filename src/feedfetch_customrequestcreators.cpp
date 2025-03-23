#include "feedfetch.h"
#include <QDebug>
#include <QMap>
#include <QNetworkRequest>

extern const QMap<const char *, std::function<QNetworkRequest(QUrl)>> customRequestCreators;

const QMap<const char *, std::function<QNetworkRequest(QUrl)>> customRequestCreators = {

    {"youtube.com",
     [](QUrl url) {
         QNetworkRequest request(url);
         request.setRawHeader("COOKIE", "SOCS=CAESEwgDEgk0ODE3Nzk3MjQaAmVuIAEaBgiA_LyaBg");
         return request;
     }},

    {"webtoons.com",
     [](QUrl url) {
         url.setPath(url.path().replace(QStringLiteral("/list"), QStringLiteral("/rss")));
         return QNetworkRequest(url);
     }},

    {"twitch.tv", [](QUrl url) {
         QString username = url.path();
         QUrl altUrl(QStringLiteral("https://twitchrss.appspot.com/vod") + username);
         qDebug() << QStringLiteral("Fetching instead") << altUrl;
         return QNetworkRequest(altUrl);
     }}};

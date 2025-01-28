#include "feedfetch.h"
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
     }}

};
